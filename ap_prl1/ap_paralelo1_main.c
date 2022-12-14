#include "ap_paralelo1_main.h"

/* the directories wher output files will be placed */
#define RESIZE_DIR "./Resize-dir/"
#define THUMB_DIR "./Thumbnail-dir/"
#define WATER_DIR "./Watermark-dir/"

gdImagePtr wm;
int n_threads=0;
char **dirs;
int n_probs=0;

int main (int argc, char *argv[]){
    if(argc!=3){
        printf("Programa mal invocado\n");
        exit(EXIT_FAILURE);
    }
    if((dirs=input_directorys(argv[1]))==NULL){
        exit(EXIT_FAILURE);
    }
    n_threads=atoi(argv[2]);
    if(n_threads<1){
        printf("Numero de threads inferior a 1\n");
        free_directorys(dirs);
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
    
    pthread_t* thread_id=(pthread_t*)malloc(n_threads*sizeof(pthread_t));
    if (thread_id==NULL){
        free_directorys(dirs);
        exit(EXIT_FAILURE);
    }
    for (n_probs=0;dirs[n_probs]!=NULL;n_probs++);

    for (long int i = 0; i < n_threads; i++)
    {
        pthread_create(&thread_id[i],NULL,ap1,(void*)i);
    }
    for (int i = 0; i < n_threads; i++)
    {
        pthread_join(thread_id[i], NULL);
    }
    gdImageDestroy(wm);
    free_directorys(dirs);
    free(thread_id);
}

void *ap1(void *arg){
    gdImagePtr t,rszd_img,thumb_img,wm_img;
    char outfilename[400];
    long int vl=(long int) arg;


    while(vl<n_probs){
        
        wm_img=NULL;
        rszd_img=NULL;
        thumb_img=NULL;
        printf("Thread %ld problem %ld\n",(long int)arg,vl);
        t=read_png_file(dirs[vl]);
        
        if(t==NULL){
            continue;
        }
        
        sprintf(outfilename,"%s%s",WATER_DIR,dirs[vl]);

        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n",outfilename);
        }
        else{
            printf("%s não encontrado\n",outfilename);
            wm_img=add_watermark(t,wm);
            if (wm_img == NULL) {
                fprintf(stderr, "Impossible to creat watermark of %s image\n", dirs[vl]);
            }else{
                if(write_png_file(wm_img,outfilename) == 0){
                    fprintf(stderr, "Impossible to write %s image\n", outfilename);
                }
            }
        }
        
        sprintf(outfilename,"%s%s",RESIZE_DIR,dirs[vl]);

        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n",outfilename);
        }
        else{
            printf("%s não encontrado\n",outfilename);
            if (wm_img==NULL){
                wm_img=add_watermark(t,wm);
            }
            rszd_img=resize_image(wm_img,800);
            if(rszd_img==NULL){
                 fprintf(stderr, "Impossible to creat resize of %s image\n", dirs[vl]);
            }else{
                if(write_png_file(rszd_img,outfilename) == 0){
                    fprintf(stderr, "Impossible to write %s image\n", outfilename);
                }
            }
        }

        sprintf(outfilename,"%s%s",THUMB_DIR,dirs[vl]);

        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n",outfilename);
        }
        else{
            printf("%s não encontrado\n",outfilename);
            if (wm_img==NULL){
                wm_img=add_watermark(t,wm);
            }
            thumb_img=make_thumb(wm_img,150);
            if(write_png_file(thumb_img,outfilename) == 0){
	            fprintf(stderr, "Impossible to write %s image\n", outfilename);
			}
        }

        gdImageDestroy(t);
        if (wm_img!=NULL)
            gdImageDestroy(wm_img);
        if (rszd_img!=NULL)
            gdImageDestroy(rszd_img);
        if (thumb_img!=NULL)
            gdImageDestroy(thumb_img);
        
        vl+=n_threads;
    }
    return NULL;
}