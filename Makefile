CC=gcc
CFLAGS=$(shell sdl2-config --cflags) -lSDL2 -lm -std=c11 -Wall -O2

chip8: core.c chip8.c
	$(CC) -o chip8.out core.c chip8.c $(CFLAGS)

debug: core.c chip8.c
	$(CC) -o chip8.out core.c chip8.c $(CFLAGS) -g

assembly: core.c chip8.c
	$(CC) -S core.c chip8.c $(CFLAGS)
