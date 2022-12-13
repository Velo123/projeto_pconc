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
void *ap1(void *arg);


#endif