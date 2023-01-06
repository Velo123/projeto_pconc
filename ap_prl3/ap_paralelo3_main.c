#include "ap_paralelo2_main.h"

gdImagePtr wm;
char **dirs;
char* RESIZE_DIR;
char* WATER_DIR;
char* THUMB_DIR;
char* arg1;

int main (int argc, char *argv[]){
    pthread_t thread_id[3];
    //verificar invocação do programa
    if(argc!=2){
        printf("Programa mal invocado\n");
        exit(EXIT_FAILURE);
    }
    //dar input aos nomes das imagens
    if((dirs=input_directorys(argv[1]))==NULL){
        exit(EXIT_FAILURE);
    } 
    if (dirs[0]==NULL){
        printf("Não foram encontradas imagens válidas\n");
        free_directorys(dirs);
        exit(EXIT_FAILURE); 
    }

    arg1=(char*)malloc(strlen(argv[1])+2);
    strcpy(arg1,argv[1]);
    strcat(arg1,"/");


    RESIZE_DIR=(char*)malloc(strlen(argv[1])+14);
    strcpy(RESIZE_DIR,argv[1]);
    strcat(RESIZE_DIR,"/Resize-dir/");
	// creation of output directories 
	if (create_directory(RESIZE_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", RESIZE_DIR);
		free_directorys(dirs);
        exit(EXIT_FAILURE); 
	}

    THUMB_DIR=(char*)malloc(strlen(argv[1])+17);
    strcpy(THUMB_DIR,argv[1]);
    strcat(THUMB_DIR,"/Thumbnail-dir/");
	if (create_directory(THUMB_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", THUMB_DIR);
		free_directorys(dirs);
        exit(EXIT_FAILURE); 
	}
    WATER_DIR=(char*)malloc(strlen(argv[1])+17);
    strcpy(WATER_DIR,argv[1]);
    strcat(WATER_DIR,"/Watermark-dir/");
	if (create_directory(WATER_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", WATER_DIR);
		free_directorys(dirs);
        exit(EXIT_FAILURE); 
	}
    //ler watermark
    char wm_loc[700];
    strcpy(wm_loc,argv[1]);
    strcat(wm_loc,"/watermark.png");
    wm = read_png_file(wm_loc);
	if(wm == NULL){
		fprintf(stderr, "Impossible to read %s image\n", "watermark.png");
		free_directorys(dirs);
        exit(EXIT_FAILURE); 
	}
    
        
    //criar as 3 threads e esperar que todas acabem
    pthread_create(&thread_id[1],NULL,thumb_thread,NULL);
    pthread_create(&thread_id[2],NULL,rz_thread,NULL);
    pthread_create(&thread_id[0],NULL,wm_thread,NULL);
    pthread_join(thread_id[0], NULL);
    pthread_join(thread_id[1], NULL);
    pthread_join(thread_id[2], NULL);
    //libertar memoria
    gdImageDestroy(wm);
    free_directorys(dirs);
    free(WATER_DIR);
    free(THUMB_DIR);
    free(RESIZE_DIR);
    free(arg1);
}


/******************************************************************************
 * rz_thread()
 *
 * Arguments: none
 * 
 * Returns: NULL
 * 
 * 
 * Side-Effects: Cria um ficheiro png com watermark e resize
 *
 * Description: esta função vai buscar os diretórios das imagens ao vetor global e cria uma versão de 
 * cada uma dessas imagens com um resize
 *
 *****************************************************************************/
void* rz_thread(void* arg){
    gdImagePtr t,rszd_img,wm_img;
    char outfilename[400];
    int vl=0;
    char infile[700];
    while(dirs[vl]!=NULL){
        wm_img=NULL;
        rszd_img=NULL;
        strcpy(infile,arg1);
        strcat(infile,dirs[vl]);
        sprintf(outfilename,"%s%s",RESIZE_DIR,dirs[vl]); 
        //verificar se já existe o ficheiro
        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n",outfilename);
        }
        else{
            printf("%s não encontrado\n",outfilename);
            //colocar watermark
            t=read_png_file(infile);  
            if(t==NULL){
                vl++;
                continue;
            }
            wm_img=add_watermark(t,wm);
            if (wm_img == NULL){
                fprintf(stderr, "Impossible to creat watermark of %s image\n", dirs[vl]);
                vl++;
                continue;
            }
            gdImageDestroy(t);
            //fazer resize
            rszd_img=resize_image(wm_img,800);
            if(rszd_img==NULL){
                fprintf(stderr, "Impossible to creat resize of %s image\n", dirs[vl]);
            }else{
                if(write_png_file(rszd_img,outfilename) == 0){
                    fprintf(stderr, "Impossible to write %s image\n", outfilename);
                }
            }
            //libertar memoria
            gdImageDestroy(rszd_img);
            gdImageDestroy(wm_img);
        }
        vl++;
    }
    return NULL;
}

/******************************************************************************
 * wm_thread()
 *
 * Arguments: none
 * 
 * Returns: NULL
 * 
 * 
 * Side-Effects: Cria um ficheiro png com watermark
 *
 * Description: esta função vai buscar os diretórios das imagens ao vetor global e cria uma versão de 
 * cada uma dessas imagens com uma watermark
 *
 *****************************************************************************/

void *wm_thread(void*arg){
    gdImagePtr t,wm_img;
    char outfilename[400];
    int vl=0;
    char infile[700];
    while(dirs[vl]!=NULL){

        wm_img=NULL;
        strcpy(infile,arg1);
        strcat(infile,dirs[vl]);
        sprintf(outfilename,"%s%s",WATER_DIR,dirs[vl]);

        if (access(outfilename,F_OK)!=-1){  //verificar se já existe
            printf("%s encontrado\n",outfilename);
        }
        else{
            //adicionar watermark
            t=read_png_file(infile);
            if(t==NULL){
                vl++;
                continue;
            }
            printf("%s não encontrado\n",outfilename);

            wm_img=add_watermark(t,wm);
            if (wm_img == NULL){
                fprintf(stderr, "Impossible to creat watermark of %s image\n", dirs[vl]);
                continue;
            }
            //criar ficheiro
            if(write_png_file(wm_img,outfilename) == 0){
                fprintf(stderr, "Impossible to write %s image\n", outfilename);
            }
            //libertar memoria
            gdImageDestroy(t);
            gdImageDestroy(wm_img);
        }
        vl++;
    }
    return NULL;
}

/******************************************************************************
 * thumb_thread()
 *
 * Arguments: none
 * 
 * Returns: NULL
 * 
 * 
 * Side-Effects: Cria um ficheiro png com watermark em thumbnail
 *
 * Description: esta função vai buscar os diretórios das imagens ao vetor global e cria uma versão de 
 * cada uma dessas imagens com uma watermark no formato de thumbnail
 *
 *****************************************************************************/


void *thumb_thread(void* arg){
    gdImagePtr t,thumb_img,wm_img;
    char outfilename[400];
    int vl=0;   
    char infile[700];
     while(dirs[vl]!=NULL){
        wm_img=NULL;
        thumb_img=NULL;
        strcpy(infile,arg1);
        strcat(infile,dirs[vl]);
        sprintf(outfilename,"%s%s",THUMB_DIR,dirs[vl]);
        //verificar se o ficheiro existe
        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n",outfilename);
        }
        else{
            //adicionar watermark
            printf("%s não encontrado\n",outfilename);
            t=read_png_file(infile);
            if(t==NULL){
                vl++;
                continue;
            }
            wm_img=add_watermark(t,wm);
            if (wm_img == NULL){
                fprintf(stderr, "Impossible to creat watermark of %s image\n", dirs[vl]);
                vl++;
                continue;
            }
            gdImageDestroy(t);
            //criar thumbnail
            thumb_img=make_thumb(wm_img,150);
            if(thumb_img==NULL){
                fprintf(stderr, "Impossible to creat thumb of %s image\n", dirs[vl]);
            }else{
                if(write_png_file(thumb_img,outfilename) == 0){
                    fprintf(stderr, "Impossible to write %s image\n", outfilename);
                }
            }
            //libertar memoria
            gdImageDestroy(thumb_img);
            gdImageDestroy(wm_img);

        }
        vl++;
    }
    return NULL;
}