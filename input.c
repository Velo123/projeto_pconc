#include"input.h"

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
        if(strstr(directory,".png")!=NULL && strcmp(directory,"watermark.png")!=0){
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
        if(strstr(directory,".png")==NULL || strcmp(directory,"watermark.png")==0)
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