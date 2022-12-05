#ifndef _input
#define _intput
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"image-lib.h"
#include"pthread.h"
#include"gd.h"

//char** input_directorys(char* filename);
gdImagePtr read_png_file(char * file_name);
int write_png_file(gdImagePtr write_img,char * file_name);


#endif