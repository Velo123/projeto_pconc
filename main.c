#include "main.h"

/* the directories wher output files will be placed */
#define RESIZE_DIR "./serial-Resize/"
#define THUMB_DIR "./serial-Thumbnail/"
#define WATER_DIR "./serial-Watermark/"

gdImagePtr wm;
int v=0;
char **dirs;

int main (int argc, char *argv[]){

    if(argc!=3){
        exit(EXIT_FAILURE);
    }
    dirs=input_directorys(argv[1]);
    int n_threads=atoi(argv[2]);

	/* creation of output directories */
	if (create_directory(RESIZE_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", RESIZE_DIR);
		exit(-1);
	}
	if (create_directory(THUMB_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", THUMB_DIR);
		exit(-1);
	}
	if (create_directory(WATER_DIR) == 0){
		fprintf(stderr, "Impossible to create %s directory\n", WATER_DIR);
		exit(-1);
	}

    wm = read_png_file("watermark.png");
	if(wm == NULL){
		fprintf(stderr, "Impossible to read %s image\n", "watermark.png");
		exit(-1);
	}
    
    pthread_t* thread_id=(pthread_t*)malloc(n_threads*sizeof(pthread_t));
    if (thread_id==NULL){
        free_directorys(dirs);
        exit(EXIT_FAILURE);
    }

    
    for (int i = 0; i < n_threads; i++)
    {
        pthread_create(thread_id[i],NULL,ap1,NULL);
    }
    for (int i = 0; i < n_threads; i++)
    {
        pthread_join(thread_id[i], NULL);
    }
    free_directorys(dirs);
    free(thread_id);
}

void *ap1(void *arg){
    gdImagePtr t,rszd_img,thumb_img,wm_img;
    char outfilename[400];
    int vl;


    while(dirs[v]!=NULL){
        vl=v;
        v++;
        wm_img=NULL;
        rszd_img=NULL;
        thumb_img=NULL;
        
        t=read_png_file(dirs[vl]);
        
        if(t==NULL){
            printf("Impossível ler imagem");
            continue;
        }
        
        sprintf(outfilename,"%s%s",WATER_DIR,dirs[vl]);

        if (access(outfilename,F_OK)!=-1){
            printf("%s encontrado\n");
        }
        else{
            printf("%s não encontrado\n");
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
            printf("%s encontrado\n");
        }
        else{
            printf("%s não encontrado\n");
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
            printf("%s encontrado\n");
        }
        else{
            printf("%s não encontrado\n");
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
        
    }   
}



