CC=gcc
CFLAGS=$(shell sdl2-config --cflags) -lSDL2 -std=c11 -O2 -Wall

chip8: core.c chip8.c
	$(CC) -o chip8.out core.c chip8.c $(CFLAGS)

debug: core.c chip8.c
	$(CC) -o chip8.out core.c chip8.c $(CFLAGS) -g
