#include"input.h"

//char** input_directorys(char* filename){

//}

gdImagePtr read_png_file(char * file_name){
    FILE * fp;
    gdImagePtr img;
    fp = fopen(file_name, "rb");
    if (!fp) {
        
    }
    img=gdImageCreateFromPng(fp);
    fclose(fp);
    if (!img) {
        return NULL;
    }
    return img;
}

int write_png_file(gdImagePtr write_img,char * file_name){
    FILE * fp;
    fp = fopen(file_name, "wb");
    if (!fp) {
        return 0;
    }
    gdImagePng(write_img, fp);
    fclose(fp);
    return 1;
 }