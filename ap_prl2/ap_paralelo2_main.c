#include "ap_paralelo2_main.h"

/* the directories wher output files will be placed */
#define RESIZE_DIR "./Resize-dir/"
#define THUMB_DIR "./Thumbnail-dir/"
#define WATER_DIR "./Watermark-dir/"

gdImagePtr wm;
gdImagePtr *wm_images_vector;
char **dirs;
pthread_mutex_t mutex;


int main (int argc, char *argv[]){
    int n_images=0;
    pthread_t thread_id[3];
    if(argc!=2){
        printf("Programa mal invocado\n");
        exit(EXIT_FAILURE);
    }
    if((dirs=input_directorys(argv[1]))==NULL){
        exit(EXIT_FAILURE);
    } 
    if (dirs[0]==NULL){
        printf("Não foram encontradas imagens válidas\n");
        free_directorys(dirs);
        exit(EXIT_FAILURE); 
    }

	/* creation of output directories */
	if (create_directory(RESIZE_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", RESIZE_DIR);
		free_directorys(dirs);
        exit(EXIT_FAILURE); 
	}
	if (create_directory(THUMB_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", THUMB_DIR);
		free_directorys(dirs);
        exit(EXIT_FAILURE); 
	}
	if (create_directory(WATER_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", WATER_DIR);
		free_directorys(dirs);
        exit(EXIT_FAILURE); 
	}

    wm = read_png_file("watermark.png");
	if(wm == NULL){
		fprintf(stderr, "Impossible to read %s image\n", "watermark.png");
		free_directorys(dirs);
        exit(EXIT_FAILURE); 
	}
    while (dirs[n_images]!=NULL)
        n_images++;
        
    wm_images_vector=(gdImagePtr*)calloc(n_images,sizeof(gdImagePtr));
    
    if(wm_images_vector==NULL){
        free_directorys(dirs);
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&mutex,NULL);
        
    pthread_create(&thread_id[0],NULL,wm_thread,NULL);
    pthread_create(&thread_id[1],NULL,thumb_thread,NULL);
    pthread_create(&thread_id[2],NULL,rz_thread,NULL);
    pthread_join(thread_id[0], NULL);
    pthread_join(thread_id[1], NULL);
    pthread_join(thread_id[2], NULL);
    gdImageDestroy(wm);
    free_directorys(dirs);
    for (int i=0;i<n_images;i++){
        if(wm_images_vector[i]!=NULL){
            gdImageDestroy(wm_images_vector[i]);
        }
    }
    free(wm_images_vector);
    pthread_mutex_destroy(&mutex);
}

void* rz_thread(void* arg){
    gdImagePtr t,rszd_img,wm_img;
    char outfilename[400];
    int vl=0;
    while(dirs[vl]!=NULL){
        wm_img=NULL;
        rszd_img=NULL;

        sprintf(outfilename,"%s%s",RESIZE_DIR,dirs[vl]);
        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n",outfilename);
        }
        else{
            if(wm_images_vector[vl]==NULL){
                t=read_png_file(dirs[vl]);
                if(t==NULL){
                    vl++;
                    continue;
                }
                printf("criou by resize\n");
                wm_img=add_watermark(t,wm);
                pthread_mutex_lock(&mutex);
                if (wm_images_vector[vl]==NULL){
                    wm_images_vector[vl]=wm_img;
                }
                else{
                    gdImageDestroy(wm_img);
                    wm_img=wm_images_vector[vl];
                }
                pthread_mutex_unlock(&mutex);
                gdImageDestroy(t);
            }
            else
                wm_img=wm_images_vector[vl];

            printf("%s não encontrado\n",outfilename);
            rszd_img=resize_image(wm_images_vector[vl],800);
            if(rszd_img==NULL){
                fprintf(stderr, "Impossible to creat resize of %s image\n", dirs[vl]);
            }else{
                if(write_png_file(rszd_img,outfilename) == 0){
                    fprintf(stderr, "Impossible to write %s image\n", outfilename);
                }
            }
            gdImageDestroy(rszd_img);
        }
        vl++;
    }
    return NULL;
}

void *wm_thread(void*arg){
    gdImagePtr t,wm_img;
    char outfilename[400];
    int vl=0;
    
    while(dirs[vl]!=NULL){

        wm_img=NULL;
        
        sprintf(outfilename,"%s%s",WATER_DIR,dirs[vl]);

        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n",outfilename);
        }
        else{
            t=read_png_file(dirs[vl]);
            if(t==NULL){
                vl++;
                continue;
            }
            printf("%s não encontrado\n",outfilename);

            if (wm_images_vector[vl]==NULL){
                printf("criou by watermark\n");
                wm_img=add_watermark(t,wm);
                if (wm_img == NULL){
                    fprintf(stderr, "Impossible to creat watermark of %s image\n", dirs[vl]);
                    continue;
                }
                pthread_mutex_lock(&mutex);
                if (wm_images_vector[vl]==NULL)
                    wm_images_vector[vl]=wm_img;
                else{
                    gdImageDestroy(wm_img);
                    wm_img=wm_images_vector[vl];
                }
                pthread_mutex_unlock(&mutex);
            }
            else 
               wm_img=wm_images_vector[vl];
                
            if(write_png_file(wm_img,outfilename) == 0){
                fprintf(stderr, "Impossible to write %s image\n", outfilename);
            }
            gdImageDestroy(t);
        }
        vl++;
    }
    return NULL;
}


void *thumb_thread(void* arg){
    gdImagePtr t,thumb_img,wm_img;
    char outfilename[400];
    int vl=0;   
     while(dirs[vl]!=NULL){
        wm_img=NULL;
        thumb_img=NULL;
        sprintf(outfilename,"%s%s",THUMB_DIR,dirs[vl]);
        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n",outfilename);
        }
        else{
            if(wm_images_vector[vl]==NULL){
                t=read_png_file(dirs[vl]);
                if(t==NULL){
                    vl++;
                    continue;
                }
                printf("criou by thumb\n");
                wm_img=add_watermark(t,wm);
                pthread_mutex_lock(&mutex);
                if (wm_images_vector[vl]==NULL){
                    wm_images_vector[vl]=wm_img;
                }
                else{
                    gdImageDestroy(wm_img);
                    wm_img=wm_images_vector[vl];
                }
                pthread_mutex_unlock(&mutex);
                gdImageDestroy(t);
            }
            else
                wm_img=wm_images_vector[vl];
            
            printf("%s não encontrado\n",outfilename);
            thumb_img=make_thumb(wm_images_vector[vl],150);
            if(thumb_img==NULL){
                fprintf(stderr, "Impossible to creat thumb of %s image\n", dirs[vl]);
            }else{
                if(write_png_file(thumb_img,outfilename) == 0){
                    fprintf(stderr, "Impossible to write %s image\n", outfilename);
                }
            }
            gdImageDestroy(thumb_img);

        }
        vl++;
    }
    return NULL;
}