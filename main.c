#include "main.h"

int v=0;
char **dirs;

int main (int argc, char *argv[]){

    if(argc!=3){
        exit(EXIT_FAILURE);
    }
    dirs=input_directorys(argv[1]);
    int n_threads=atoi(argv[2]);
    
    
    /*pthread_t thread_id[n_threads];
    
    for (int i = 0; i < n_threads; i++)
    {
        pthread_create(thread_id[i],NULL,ap1,NULL);
    }
    for (int i = 0; i < n_threads; i++)
    {
        pthread_join(thread_id[i], NULL);
    }*/
    free_directorys(dirs);
}

/*void *ap1(void *arg){
    gdImagePtr t;
    while(dirs[v]!=NULL){
        t=read_png_file(dirs[v]);
        add_watermark(t,NULL);
        
        v++;
    }
    
}*/



