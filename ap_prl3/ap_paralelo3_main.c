#include "ap_paralelo3_main.h"
#define MAX_LEN 400


typedef struct _data{
    gdImagePtr image;
    char location[400];
}data;


gdImagePtr wm;
int pipewm[2];
int pipethumb[2];
int piperez[2];
char **dirs;
char* RESIZE_DIR;
char* WATER_DIR;
char* THUMB_DIR;
char* arg1;
int v=0;

int main (int argc, char *argv[]){
    int n_threads=0;

    pthread_t** thread_id;
    //verificar invocação do programa
    if(argc!=3){
        printf("Programa mal invocado\n");
        exit(EXIT_FAILURE);
    }
    n_threads=atoi(argv[1]);
    if(n_threads<1){
        printf("Programa mal invocado\n");
        exit(EXIT_FAILURE);
    }
    //dar input aos nomes das imagens
    if((dirs=input_directorys(argv[2]))==NULL){
        exit(EXIT_FAILURE);
    } 
    if (dirs[0]==NULL){
        printf("Não foram encontradas imagens válidas\n");
        free_directorys(dirs);
        exit(EXIT_FAILURE); 
    }

    thread_id=(pthread_t**)malloc(sizeof(pthread_t*)*n_threads);
    if (thread_id==NULL)
    {
        printf("Erro na alocação 1\n");
        free_directorys(dirs);
        exit(EXIT_FAILURE); 
    }

    for (int i = 0; i < n_threads; i++)
    {
        thread_id[i]=(pthread_t*)malloc(sizeof(pthread_t)*3);
        if (thread_id[i]==NULL)
        {
            printf("Erro na alocação 2\n");
            free(thread_id);
            free_directorys(dirs);
            exit(EXIT_FAILURE); 
        }
    }
    pipe(pipewm);
    pipe(pipethumb);
    pipe(piperez);

    arg1=(char*)malloc(strlen(argv[2])+2);
    strcpy(arg1,argv[2]);
    strcat(arg1,"/");


    RESIZE_DIR=(char*)malloc(strlen(argv[2])+14);
    strcpy(RESIZE_DIR,argv[2]);
    strcat(RESIZE_DIR,"/Resize-dir/");
	// creation of output directories 
	if (create_directory(RESIZE_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", RESIZE_DIR);
		free_directorys(dirs);
        exit(EXIT_FAILURE); 
	}

    THUMB_DIR=(char*)malloc(strlen(argv[2])+17);
    strcpy(THUMB_DIR,argv[2]);
    strcat(THUMB_DIR,"/Thumbnail-dir/");
	if (create_directory(THUMB_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", THUMB_DIR);
		free_directorys(dirs);
        exit(EXIT_FAILURE); 
	}
    WATER_DIR=(char*)malloc(strlen(argv[2])+17);
    strcpy(WATER_DIR,argv[2]);
    strcat(WATER_DIR,"/Watermark-dir/");
	if (create_directory(WATER_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", WATER_DIR);
		free_directorys(dirs);
        exit(EXIT_FAILURE); 
	}
    //ler watermark
    char wm_loc[700];
    strcpy(wm_loc,argv[2]);
    strcat(wm_loc,"/watermark.png");
    wm = read_png_file(wm_loc);
	if(wm == NULL){
		fprintf(stderr, "Impossible to read %s image\n", "watermark.png");
		free_directorys(dirs);
        exit(EXIT_FAILURE); 
	}
    //criar todas as threads necessárias
    for (long int i = 0; i < n_threads; i++)
    {
        pthread_create(&thread_id[i][0],NULL,wm_thread,NULL);
        pthread_create(&thread_id[i][1],NULL,thumb_thread,NULL);
        pthread_create(&thread_id[i][2],NULL,rz_thread,NULL);
    }

    //escrever nomes das imagens nos pipes
    for (int i = 0; dirs[i]!=NULL ; i++)
    {
        write(pipewm[1],dirs[i],MAX_LEN*sizeof(char));
    }

    //escrever fim de ficheiros
    for (int i = 0; i < n_threads; i++)
    {
        write(pipewm[1],"",MAX_LEN*sizeof(char));
    }
    
    //esperar que todas as threads terminem
    for (int i = 0; i < n_threads; i++)
    {
        pthread_join(thread_id[i][0], NULL);
        pthread_join(thread_id[i][1], NULL);
        pthread_join(thread_id[i][2], NULL);
    }
    
    //libertar memoria
    for (int i = 0; i < n_threads; i++)
    {
        free(thread_id[i]);
    }
    free(thread_id);
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
    gdImagePtr rszd_img;
    char outfilename[400];
    char infile[700];
    data pipe_info;
    while(1){
        read(piperez[0],&pipe_info,sizeof(data));
        if (pipe_info.image==NULL)
            break;
        rszd_img=NULL;
        strcpy(infile,arg1);
        strcat(infile,pipe_info.location);
        sprintf(outfilename,"%s%s",RESIZE_DIR,pipe_info.location); 
        //verificar se já existe o ficheiro
        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n",outfilename);
        }
        else{
            printf("%s não encontrado\n",outfilename);
            
            //fazer resize
            rszd_img=resize_image(pipe_info.image,800);
            if(rszd_img==NULL){
                fprintf(stderr, "Impossible to creat resize of %s image\n", pipe_info.location);
            }else{
                if(write_png_file(rszd_img,outfilename) == 0){
                    fprintf(stderr, "Impossible to write %s image\n", outfilename);
                }
            }
            //libertar memoria
            gdImageDestroy(rszd_img);
        }
        gdImageDestroy(pipe_info.image);
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
    char infile[700];
    char fileinput[MAX_LEN];
    data pipe_info;
    
    while(1){
        read(pipewm[0],&fileinput,sizeof(char)*MAX_LEN);
        if (strlen(fileinput)==0)
            break;
        wm_img=NULL;
        strcpy(infile,arg1);
        strcat(infile,fileinput);
        sprintf(outfilename,"%s%s",WATER_DIR,fileinput);

        if (access(outfilename,F_OK)!=-1){  //verificar se já existe
            printf("%s encontrado\n",outfilename);
            wm_img=read_png_file(outfilename);
        }
        else{
            //adicionar watermark
            t=read_png_file(infile);
            if(t==NULL){
                continue;
            }
            printf("%s não encontrado\n",outfilename);

            wm_img=add_watermark(t,wm);
            if (wm_img == NULL){
                fprintf(stderr, "Impossible to creat watermark of %s image\n", fileinput);
                continue;
            }
            //criar ficheiro
            if(write_png_file(wm_img,outfilename) == 0){
                fprintf(stderr, "Impossible to write %s image\n", outfilename);
            }
            //libertar memoria
            gdImageDestroy(t);
        }
        pipe_info.image=wm_img;
        strcpy(pipe_info.location,fileinput);
        write(pipethumb[1],&pipe_info,sizeof(data));
    }
    pipe_info.image=NULL;
    write(pipethumb[1],&pipe_info,sizeof(data));

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
    gdImagePtr thumb_img;
    char outfilename[400];
    char infile[700];
    data pipeinfo;
    while(1){
        read(pipethumb[0],&pipeinfo,sizeof(data));
        if (pipeinfo.image==NULL)
            break;
        thumb_img=NULL;
        strcpy(infile,arg1);
        strcat(infile,pipeinfo.location);
        sprintf(outfilename,"%s%s",THUMB_DIR,pipeinfo.location);
        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n",outfilename);
        }
        else{
            printf("%s não encontrado\n",outfilename);
            //criar thumbnail
            thumb_img=make_thumb(pipeinfo.image,150);
            if(thumb_img==NULL){
                fprintf(stderr, "Impossible to creat thumb of %s image\n", pipeinfo.location);
            }else{
                if(write_png_file(thumb_img,outfilename) == 0){
                    fprintf(stderr, "Impossible to write %s image\n", outfilename);
                }
            }
            //libertar memoria
            gdImageDestroy(thumb_img);
        }
        write(piperez[1],&pipeinfo,sizeof(data));
    }
    pipeinfo.image=NULL;
    write(piperez[1],&pipeinfo,sizeof(data));
    return NULL;
}