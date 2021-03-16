#include "cpu.h"

typedef struct Chip8
{
    Chip8CPU CPU;

    unsigned short opcode;
 
    unsigned char mem[4096];
    unsigned char graphics[2048];
    unsigned char keyboard[16];

    unsigned char delaytimer;
    unsigned char soundtimer;

} Chip8;

void cpu_cycle(Chip8 chip8);
void reset(Chip8 chip8);
void keypress(Chip8 chip8);
void load_game(Chip8 chip8, const char* title);
