#ifndef ap_paralelo1_main
#define ap_paralelo1_main
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"image-lib.h"
#include"pthread.h"
#include"gd.h"
#include"input.h"
#include<unistd.h>

int main (int argc, char *argv[]);
char** input_directorys(char* filename);
void free_directorys(char** directorys);
void* rz_thread(void* arg);
void *thumb_thread(void* arg);
void *wm_thread(void*arg);


#endif