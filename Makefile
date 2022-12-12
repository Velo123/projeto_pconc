
#Makefile ap-paralelo-1
CFLAGS  = -Wall -pedantic -g -pthread -lpthread

default: all

all:main.o image-lib.o input.o
	gcc $(CFLAGS) -o ap-paralelo-1 main.o image-lib.o input.o -lgd

main.o: main.c main.h
	gcc  $(CFLAGS) -c main.c

image-lib.o: image-lib.c image-lib.h
	gcc  $(CFLAGS) -c image-lib.c

input.o: input.c input.h
	gcc  $(CFLAGS) -c input.c

	
clean:
	rm -rf *o all
	rm -rf *o ap-paralelo-1
