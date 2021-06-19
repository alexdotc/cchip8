#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MEMSIZE 4096
#define GFXSIZE 2048
#define STACKSIZE 16

typedef struct
{
    unsigned int speed;
    unsigned int scale;
    const char *rom;
} Options;

typedef struct
{
    const uint16_t memsize;
    const uint16_t gfxsize;
    const uint8_t stacksize;

    uint16_t PC; // program counter
    uint16_t I; // address register
    uint16_t SP; // stack pointer

    uint8_t DT; // delay timer
    uint8_t ST; // sound timer

    uint8_t V[16]; // data registers
    uint16_t stack[STACKSIZE];
 
    uint8_t mem[MEMSIZE];
    uint8_t gfx[GFXSIZE];
    uint8_t key[16];

    bool draw_cycle;

} Chip8;

int cycle(Chip8 *chip8);
void reset(Chip8 *chip8);
void dec_timers(Chip8 *chip8);
void keypress(Chip8 *chip8);
int load_rom(Chip8 *chip8, const char *path);

int init_platform(Options *options);
int update_platform(Chip8 *chip8);
