#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chip8.h"

#define FONT_START 0x000
#define PROG_START 0x200

static inline uint8_t decode_vx(uint16_t opcode);
static inline uint8_t decode_vy(uint16_t opcode);
static inline uint8_t decode_n(uint16_t opcode);
static inline uint8_t decode_nn(uint16_t opcode);
static inline uint16_t decode_nnn(uint16_t opcode);

static inline void SYS_Addr();
static inline void CLS(Chip8 *chip8);
static inline void RET(Chip8 *chip8);
static inline void DRW(Chip8 *chip8, uint16_t opcode);
static inline void JP_Addr(Chip8 *chip8, uint16_t opcode);
static inline void LD_Vx_Byte(Chip8 *chip8, uint16_t opcode);
static inline void LD_I_Addr(Chip8 *chip, uint16_t opcode);
static inline void ADD_Vx_Byte(Chip8 *chip8, uint16_t opcode);

static const uint8_t fonts[80] =
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
    fclose(f);

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
                         default: SYS_Addr(); break;
                     } break;
        case 0x1000: JP_Addr(chip8, opcode); break;
        case 0x6000: LD_Vx_Byte(chip8, opcode); break;
        case 0x7000: ADD_Vx_Byte(chip8, opcode); break;
        case 0xA000: LD_I_Addr(chip8, opcode); break;
        case 0xD000: DRW(chip8, opcode); break;
        default: fprintf(stderr, "cycle(): Unkown opcode: %x\n", opcode); return EXIT_FAILURE;
    }
    return 0;
}

static inline uint8_t decode_vx(uint16_t opcode)
{
    return (opcode & 0x0F00) >> 8;
}

static inline uint8_t decode_vy(uint16_t opcode)
{
    return (opcode & 0x00F0) >> 4;
}

static inline uint8_t decode_n(uint16_t opcode)
{
    return opcode & 0x000F;
}

static inline uint8_t decode_nn(uint16_t opcode)
{
    return opcode & 0x00FF;
}

static inline uint16_t decode_nnn(uint16_t opcode)
{
    return opcode & 0x0FFF;
}

static inline void SYS_Addr()
{
    // deliberately ignored, not used on modern CHIP-8 interpreters
    return;
}

static inline void CLS(Chip8 *chip8)
{
    memset(chip8->gfx, 0, chip8->gfxsize);
    chip8->draw_cycle = true;
    return;
}

static inline void RET(Chip8 *chip8)
{   // TODO
    return;
}

static inline void JP_Addr(Chip8 *chip8, uint16_t opcode)
{
    chip8->PC = decode_nnn(opcode);
    return;
}

static inline void LD_Vx_Byte(Chip8 *chip8, uint16_t opcode)
{
    uint8_t vx = decode_vx(opcode);
    uint8_t byte = decode_nn(opcode);
    chip8->V[vx] = byte;
    return;
}

static inline void ADD_Vx_Byte(Chip8 *chip8, uint16_t opcode)
{
    uint8_t vx = decode_vx(opcode);
    uint8_t byte = decode_nn(opcode);
    chip8->V[vx] += byte;
    return;
}

static inline void LD_I_Addr(Chip8 *chip8, uint16_t opcode)
{
    chip8->I = decode_nnn(opcode);
    return;
}

static inline void DRW(Chip8 *chip8, uint16_t opcode)
{
    uint8_t vx = decode_vx(opcode);
    uint8_t vy = decode_vy(opcode);
    uint8_t nibble = decode_n(opcode);
    uint8_t dx = chip8->V[vx] % 64;
    uint8_t dy = chip8->V[vy] % 32;
    uint8_t sb; // 1 byte of a sprite
    uint8_t px;
    uint8_t *dest;
    int m;

    chip8->V[0xF] = 0;

    for(uint8_t offset = 0; offset < nibble; ++offset){
        sb = chip8->mem[chip8->I + offset];
        m = 0;
        for(int i = 0; i < 8; i++){
            dest = (chip8->gfx)+(64*dy + dx);
            px = (sb >> (7-i)) & 1;
            if (*dest && px) chip8->V[0xF] = 1; // collision
            *dest ^= px;
            dx++;
            m++;
            if (dx > 63) break;
        }
        dy++;
        if (dy > 31) break;
        dx -= m;
    }
    chip8->draw_cycle = true;
    return;
}