CC=gcc
CFLAGS=$(shell sdl2-config --cflags) -lSDL2 -lm -std=c11 -Wall -O2
SRCDIR=src

chip8:
	cd $(SRCDIR) && \
	$(CC) -o ../chip8.out core.c chip8.c platform.c $(CFLAGS)

debug:
	cd $(SRCDIR) && \
	$(CC) -o chip8.out core.c chip8.c platform.c $(CFLAGS) -g
