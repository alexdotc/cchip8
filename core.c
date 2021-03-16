#include "chip8.h"

int main(int argc, char **argv)
{
    // set up graphics
    // set up input

    Chip8 chip8;

    reset(chip8);
    load_game(chip8, "pong");

    while(1)
    {
	    cpu_cycle(chip8);
    }
    return 0;
}
