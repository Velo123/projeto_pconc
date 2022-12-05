#include "main.h"

int v=0;
char **dirs;
#define WM ""

int main (int argc, char *argv[]){

    if(argc!=3){
        exit(EXIT_FAILURE);
    }
    dirs=input_directorys(argv[1]);
    int n_threads=atoi(argv[2]);
    
    for (int i=0;dirs[i]!=NULL;i++){
        printf("%s\n",dirs[i]);
    }
    /*
    pthread_t thread_id[n_threads];
    
    for (int i = 0; i < n_threads; i++)
    {
        pthread_create(thread_id[i],NULL,ap1,NULL);
    }
    for (int i = 0; i < n_threads; i++)
    {
        pthread_join(thread_id[i], NULL);
    }
    */
    free_directorys(dirs);
    


}

char** input_directorys(char* filename){
    char directory[700],**directorys;
    int n_images=0,i=0;
    FILE *fin=fopen(filename,"r");

    if (fin==NULL){
        printf("Error opening the file:%s",filename);
        return NULL;
    }
    while (!feof(fin)){
        fgets(directory,700,fin);
        if(strstr(directory,".png")!=NULL){
            n_images++;
        }
    }
    
    fseek(fin,0,SEEK_SET);

    directorys=(char**)malloc((n_images+1)*sizeof(char*));
    if (directorys==NULL){
        fclose(fin);
        return NULL;
    }
    directorys[n_images]=NULL;
    
    while (!feof(fin)){
        fgets(directory,700,fin);
        if(strstr(directory,".png")==NULL)
            continue;
        else{
            directorys[i]=malloc((strlen(directory)+1)*sizeof(char));
            if (directorys[i]==NULL){
                fclose(fin);
                free_directorys(directorys);
            }
            strcpy(directorys[i],directory);
            i++;
        }
    }
    fclose(fin);
    return directorys;
}

void free_directorys(char** directorys){
    int i;
    for (i=0;directorys[i]!=NULL;i++)
        free(directorys[i]);
    free(directorys);
}


/*void *ap1(void *arg){
    gdImagePtr t;
    while(dirs[v]!=NULL){
        t=read_png_file(dirs[v]);
        add_watermark(t,NULL);
        
        v++;
    }
    
}*/



