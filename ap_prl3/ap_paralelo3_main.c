#include "ap_paralelo3_main.h"
#define MAX_LEN 400

//estrutura para guardar a imagem e a sua localização
typedef struct _data{
    gdImagePtr image;
    int location;
}data;


gdImagePtr wm;
int pipewm[2]={0};
int pipethumb[2]={0};
int piperez[2]={0};
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
    n_threads=atoi(argv[2]);
    if(n_threads<1){
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
    //alocar memoria para as threads
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
    
    //criar pipes
    pipe(pipewm);
    pipe(pipethumb);
    pipe(piperez);

    //criar directorias
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
        write(pipewm[1],&i,sizeof(int));
    }
    int a=-1;
    //escrever fim de ficheiros
    for (int i = 0; i < n_threads; i++)
    {
        write(pipewm[1],&a,sizeof(int));
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
 * Description: esta função vai buscar uma imagem com watermark ao pipe e cria uma versão dela com watermark e resize 
 * 
 *
 *****************************************************************************/
void* rz_thread(void* arg){
    gdImagePtr rszd_img;
    char outfilename[400];
    char infile[700];
    data pipe_info;
    while(1){
        //ler pipe
        read(piperez[0],&pipe_info,sizeof(data));
        if (pipe_info.location==-1)
            break;
        rszd_img=NULL;
        strcpy(infile,arg1);
        strcat(infile,dirs[pipe_info.location]);
        sprintf(outfilename,"%s%s",RESIZE_DIR,dirs[pipe_info.location]); 
        //verificar se já existe o ficheiro
        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n",outfilename);
        }
        else{
            printf("%s não encontrado\n",outfilename);
            
            //fazer resize
            rszd_img=resize_image(pipe_info.image,800);
            if(rszd_img==NULL){
                fprintf(stderr, "Impossible to creat resize of %s image\n", dirs[pipe_info.location]);
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
 * Description: esta função vai buscar os diretórios das imagens a um pipe e cria uma versão dela com watermark
 * 
 *
 *****************************************************************************/

void *wm_thread(void*arg){
    gdImagePtr t,wm_img;
    char outfilename[400];
    char infile[700];
    int vl=0;
    data pipe_info;
    
    while(1){
        //ler pipe
        read(pipewm[0],&vl,sizeof(int));
        if (vl==-1)
            break;
        wm_img=NULL;
        strcpy(infile,arg1);
        strcat(infile,dirs[vl]);
        sprintf(outfilename,"%s%s",WATER_DIR,dirs[vl]);

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
                fprintf(stderr, "Impossible to creat watermark of %s image\n", dirs[vl]);
            }
            //criar ficheiro
            if(write_png_file(wm_img,outfilename) == 0){
                fprintf(stderr, "Impossible to write %s image\n", outfilename);
            }
            //libertar memoria
            gdImageDestroy(t);
        }
        //escrever pipe
        pipe_info.image=wm_img;
        pipe_info.location=vl;
        write(pipethumb[1],&pipe_info,sizeof(data));
    }
    pipe_info.location=-1;
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
 * Description: esta função vai buscar os diretórios das imagens a um pipe e cria uma versão dela com watermark em thumbnail
 * 
 *
 *****************************************************************************/

void *thumb_thread(void* arg){
    return NULL;
    gdImagePtr thumb_img;
    char outfilename[400];
    char infile[700];
    data pipeinfo;
    while(1){
        //ler pipe
        read(pipethumb[0],&pipeinfo,sizeof(data));
        if (pipeinfo.location==-1)
            break;
        thumb_img=NULL;
        strcpy(infile,arg1);
        strcat(infile,dirs[pipeinfo.location]);
        sprintf(outfilename,"%s%s",THUMB_DIR,dirs[pipeinfo.location]);
        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n",outfilename);
        }
        else{
            printf("%s não encontrado\n",outfilename);
            //criar thumbnail
            thumb_img=make_thumb(pipeinfo.image,150);
            if(thumb_img==NULL){
                fprintf(stderr, "Impossible to creat thumb of %s image\n", dirs[pipeinfo.location]);
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