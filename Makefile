# Very, very sophisticated makefile.


all: *.c
	gcc -g *.c -o queens -Wall

fast: *.c
	gcc *.c -o queens -Wall