# Very, very sophisticated makefile.


all: *.c
	gcc -g *.c -o queens -Wall -lX11 -lXtst

fast: *.c
	gcc *.c -o queens -Wall -O3 -lX11 -lXtst