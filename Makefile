CC=gcc
CFLAGS=-I/usr/include/SDL2 -D_REENTRANT -lSDL2

chip8: core.c chip8.c
	$(CC) -o chip8.out core.c chip8.c $(CFLAGS)
