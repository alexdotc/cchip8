#include <stdint.h>

typedef struct Chip8
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
    uint16_t stack[16];
 
    uint8_t mem[4096];
    uint8_t gfx[2048];
    uint8_t key[16];

} Chip8;

void cycle(Chip8 *chip8);
void reset(Chip8 *chip8);
void keypress(Chip8 *chip8);
int load_rom(Chip8 *chip8, const char *path);
