CC=gcc

chip8: core.c chip8.c
	$(CC) -o chip8.out core.c chip8.c
