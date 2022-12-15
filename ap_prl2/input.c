#include"input.h"

/******************************************************************************
 * input_directorys()
 *
 * Arguments: filename - string com o nome do ficheiro que contem os diretórios com as
 *                       imagens a tratar
 * 
 * Returns: vector de strings com os diretorios de todas as imagens a resolver com o ultimo endereço de 
 *          string a nulo
 * Side-Effects: Aloca memória para guardadar os dirétorios
 *
 * Description: esta função lê todo o ficheiro de diretorios conta o numero de diretorios existentes
 *              válidos e guarda-os num vetor de diretórios que retorna
 *
 *****************************************************************************/

char** input_directorys(char* filename){
    char directory[700],**directorys;
    int n_images=0,i=0;
    FILE *fin=fopen(filename,"r");
    //comfirma a existencia do ficheiro
    if (fin==NULL){
        printf("Error opening the file:%s\n",filename);
        return NULL;
    }
    //percorre todo o ficheiro e conta o numero de diretorios válidos
    while (!feof(fin)){
        fscanf(fin,"%s\n",directory);
        if(strstr(directory,".png")!=NULL && strcmp(directory,"watermark.png")!=0){
            n_images++;
        }
    }
    
    fseek(fin,0,SEEK_SET);
    //Aloca a memória nesseçária para guardar todos os diretorios 
    directorys=(char**)malloc((n_images+1)*sizeof(char*));
    if (directorys==NULL){
        fclose(fin);
        return NULL;
    }
    //marca a ultima casa do vetor a nulo
    directorys[n_images]=NULL;

    ///percorre todo o ficheiro de novo e guarda os dirétorios válidos
    while (!feof(fin)){
        fscanf(fin,"%s\n",directory);
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
/******************************************************************************
 * free_directorys()
 *
 * Arguments: directorys-vetor de strings com diretorios guardados criado pela função input_directorys
 * 
 * Returns: none
 * Side-Effects: liberta toda a memória do vetor incluido o proprio vetor
 *
 * Description: esta função liberta toda a memória do vetor diretoŕios enviado;
 *
 *****************************************************************************/
void free_directorys(char** directorys){
    int i;
    for (i=0;directorys[i]!=NULL;i++)
        free(directorys[i]);
    free(directorys);
}