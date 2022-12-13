#ifndef _input
#define _intput
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"image-lib.h"
#include"pthread.h"
#include"gd.h"

char** input_directorys(char* filename);
void free_directorys(char** directorys);

#endif