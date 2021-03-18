#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"

#define FONT_START 0x000
#define PROG_START 0x200

void SYS_addr();
void CLS(Chip8 *chip8);
void RET(Chip8 *chip8);
void LD_Vx_Byte(Chip8 *chip8, uint16_t opcode);

const uint8_t fonts[80] =
{ 
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

int load_rom(Chip8 *chip8, const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL){
        perror("Load ROM failure: fopen()");
        return EXIT_FAILURE;
    }
    fseek(f, 0, SEEK_END);
    uint16_t len = ftell(f);
    rewind(f);
    fread(chip8->mem+PROG_START, len, 1, f);

    return 0;
}

void reset(Chip8 *chip8)
{
    chip8->PC = PROG_START;
    chip8->I = 0;
    chip8->SP = 0;
    chip8->DT = 0;
    chip8->DT = 0;

    memset(chip8->mem, 0, chip8->memsize);
    memset(chip8->gfx, 0, chip8->gfxsize);
    memset(chip8->key, 0, 16);
    memset(chip8->stack, 0, chip8->stacksize*sizeof(uint16_t));
    memset(chip8->V, 0, 16*sizeof(uint16_t));

    memcpy(chip8->mem+FONT_START, fonts, sizeof(fonts));

    chip8->draw_cycle = false;

    return;
}

int cycle(Chip8 *chip8)
{
    uint16_t opcode = chip8->mem[chip8->PC] << 8 | chip8->mem[chip8->PC+1];
    chip8->PC += 2;
    chip8->draw_cycle = false;

    switch(opcode & 0xF000){
        case 0x0000: switch(opcode & 0x00FF){
	                 case 0x00E0: CLS(chip8); break;
                         case 0x00EE: RET(chip8); break;
                         default: SYS_addr(); break;
                     } break;
        case 0x6000: LD_Vx_Byte(chip8, opcode); break;
        default: fprintf(stderr, "cycle(): Unkown opcode: %x\n", opcode); return EXIT_FAILURE;
    }
    return 0;
}

void SYS_addr()
{
    // deliberately ignored, not useful on a modern interpreter
    return;
}

void CLS(Chip8 *chip8)
{
    memset(chip8->gfx, 0, chip8->gfxsize);
    chip8->draw_cycle = true;
    return;
}

void RET(Chip8 *chip8)
{   // TODO
    return;
}

void LD_Vx_Byte(Chip8 *chip8, uint16_t opcode)
{
    // TODO
    return;
}
