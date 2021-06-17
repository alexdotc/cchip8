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
static inline void CALL(Chip8 *chip8, uint16_t opcode);
static inline void DRW(Chip8 *chip8, uint16_t opcode);
static inline void JP_Addr(Chip8 *chip8, uint16_t opcode);
static inline void SE_Vx_Byte(Chip8 *chip8, uint16_t opcode);
static inline void SNE_Vx_Byte(Chip8 *chip8, uint16_t opcode);
static inline void SE_Vx_Vy(Chip8 *chip8, uint16_t opcode);
static inline void LD_Vx_Byte(Chip8 *chip8, uint16_t opcode);
static inline void LD_I_Addr(Chip8 *chip, uint16_t opcode);
static inline void ADD_Vx_Byte(Chip8 *chip8, uint16_t opcode);
static inline void LD_Vx_Vy(Chip8 *chip8, uint16_t opcode);
static inline void OR_Vx_Vy(Chip8 *chip8, uint16_t opcode);
static inline void AND_Vx_Vy(Chip8 *chip8, uint16_t opcode);
static inline void XOR_Vx_Vy(Chip8 *chip8, uint16_t opcode);
static inline void ADD_Vx_Vy(Chip8 *chip8, uint16_t opcode);
static inline void SUB_Vx_Vy(Chip8 *chip8, uint16_t opcode);
static inline void SHR_Vx_Vy(Chip8 *chip8, uint16_t opcode);
static inline void SHL_Vx_Vy(Chip8 *chip8, uint16_t opcode);
static inline void SUBN_Vx_Vy(Chip8 *chip8, uint16_t opcode);
static inline void SNE_Vx_Vy(Chip8 *chip8, uint16_t opcode);
static inline void RND_Vx_Byte(Chip8 *chip8, uint16_t opcode);
static inline void SKP_Vx(Chip8 *chip8, uint16_t opcode);
static inline void SKNP_Vx(Chip8 *chip8, uint16_t opcode);
static inline void JP_V0_Addr(Chip8 *chip8, uint16_t opcode);
static inline void LD_DT_Vx(Chip8 *chip8, uint16_t opcode);
static inline void LD_Vx_DT(Chip8 *chip8, uint16_t opcode);
static inline void LD_Vx_Key(Chip8 *chip8, uint16_t opcode);
static inline void LD_ST_Vx(Chip8 *chip8, uint16_t opcode);
static inline void ADD_I_Vx(Chip8 *chip8, uint16_t opcode);
static inline void LD_I_FontVx(Chip8 *chip8, uint16_t opcode);
static inline void LD_I_BCDVx(Chip8 *chip8, uint16_t opcode);
static inline void LD_ArrayI_Vx(Chip8 *chip8, uint16_t opcode);
static inline void LD_ArrayVx_I(Chip8 *chip8, uint16_t opcode);


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

    srand((unsigned int)time(NULL));

    return;
}

void dec_timers(Chip8 *chip8)
{
    if (chip8->DT) --chip8->DT;
    if (chip8->ST) --chip8->ST;
    return;
}

int cycle(Chip8 *chip8)
{
    uint16_t opcode = chip8->mem[chip8->PC] << 8 | chip8->mem[chip8->PC+1];
    chip8->PC += 2;
    chip8->draw_cycle = false;

    switch(opcode & 0xF000){
        case 0x0000: switch(decode_nnn(opcode)){
	                 case 0x00E0: CLS(chip8); break;
                     case 0x00EE: RET(chip8); break;
                     default: SYS_Addr(); break;
                     } break;
        case 0x1000: JP_Addr(chip8, opcode); break;
        case 0x2000: CALL(chip8, opcode); break;
        case 0x3000: SE_Vx_Byte(chip8, opcode); break;
        case 0x4000: SNE_Vx_Byte(chip8, opcode); break;
        case 0x5000: SE_Vx_Vy(chip8, opcode); break;
        case 0x6000: LD_Vx_Byte(chip8, opcode); break;
        case 0x7000: ADD_Vx_Byte(chip8, opcode); break;
        case 0x8000: switch(decode_n(opcode)){
                     case 0x0000: LD_Vx_Vy(chip8, opcode); break;
                     case 0x0001: OR_Vx_Vy(chip8, opcode); break;
                     case 0x0002: AND_Vx_Vy(chip8, opcode); break;
                     case 0x0003: XOR_Vx_Vy(chip8, opcode); break;
                     case 0x0004: ADD_Vx_Vy(chip8, opcode); break;
                     case 0x0005: SUB_Vx_Vy(chip8, opcode); break;
                     case 0x0006: SHR_Vx_Vy(chip8, opcode); break;
                     case 0x0007: SUBN_Vx_Vy(chip8, opcode); break;
                     case 0x000E: SHL_Vx_Vy(chip8, opcode); break;
                     } break;
        case 0x9000: SNE_Vx_Vy(chip8, opcode); break;
        case 0xA000: LD_I_Addr(chip8, opcode); break;
        case 0xB000: JP_V0_Addr(chip8, opcode); break;
        case 0xC000: RND_Vx_Byte(chip8, opcode); break;
        case 0xD000: DRW(chip8, opcode); break;
        case 0xE000: switch(decode_nn(opcode)){
                     case 0x009E: SKP_Vx(chip8, opcode); break;
                     case 0x00A1: SKNP_Vx(chip8, opcode); break;
                     } break;
        case 0xF000: switch(decode_nn(opcode)){
                     case 0x0007: LD_Vx_DT(chip8, opcode); break;
                     case 0x000A: LD_Vx_Key(chip8, opcode); break;
                     case 0x0015: LD_DT_Vx(chip8, opcode); break;
                     case 0x0018: LD_ST_Vx(chip8, opcode); break;
                     case 0x001E: ADD_I_Vx(chip8, opcode); break;
                     case 0x0029: LD_I_FontVx(chip8, opcode); break;
                     case 0x0033: LD_I_BCDVx(chip8, opcode); break;
                     case 0x0055: LD_ArrayI_Vx(chip8, opcode); break;
                     case 0x0065: LD_ArrayVx_I(chip8, opcode); break;
                     } break;
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
{   
    chip8->SP--; // see comments in CALL
    chip8->PC = chip8->stack[chip8->SP];
    return;
}

static inline void JP_Addr(Chip8 *chip8, uint16_t opcode)
{
    chip8->PC = decode_nnn(opcode);
    return;
}

static inline void CALL(Chip8 *chip8, uint16_t opcode)
{
    chip8->stack[chip8->SP] = chip8->PC; // this must be right
    chip8->SP++; // but these actions are reversed from Cowgod's (?)
    chip8->PC = decode_nnn(opcode);
    return;
}

static inline void SE_Vx_Byte(Chip8 *chip8, uint16_t opcode)
{
    if(chip8->V[decode_vx(opcode)] == decode_nn(opcode)) chip8->PC += 2;
    return;
}

static inline void SNE_Vx_Byte(Chip8 *chip8, uint16_t opcode)
{
    if(chip8->V[decode_vx(opcode)] != decode_nn(opcode)) chip8->PC += 2;
    return;
}

static inline void SE_Vx_Vy(Chip8 *chip8, uint16_t opcode)
{
    if(chip8->V[decode_vx(opcode)] == chip8->V[decode_vy(opcode)]) chip8->PC += 2;
    return;
}

static inline void SNE_Vx_Vy(Chip8 *chip8, uint16_t opcode)
{
    if(chip8->V[decode_vx(opcode)] != chip8->V[decode_vy(opcode)]) chip8->PC += 2;
    return;
}

static inline void LD_Vx_Byte(Chip8 *chip8, uint16_t opcode)
{
    chip8->V[decode_vx(opcode)] = decode_nn(opcode);
    return;
}

static inline void ADD_Vx_Byte(Chip8 *chip8, uint16_t opcode)
{
    // this instruction does not affect carry flag
    chip8->V[decode_vx(opcode)] += decode_nn(opcode);
    return;
}

static inline void LD_Vx_Vy(Chip8 *chip8, uint16_t opcode)
{
    chip8->V[decode_vx(opcode)] = chip8->V[decode_vy(opcode)];
    return;
}

static inline void OR_Vx_Vy(Chip8 *chip8, uint16_t opcode)
{
    chip8->V[decode_vx(opcode)] |= chip8->V[decode_vy(opcode)];
    return;
}

static inline void AND_Vx_Vy(Chip8 *chip8, uint16_t opcode)
{
    chip8->V[decode_vx(opcode)] &= chip8->V[decode_vy(opcode)];
    return;
}

static inline void XOR_Vx_Vy(Chip8 *chip8, uint16_t opcode)
{
    chip8->V[decode_vx(opcode)] ^= chip8->V[decode_vy(opcode)];
    return;
}

static inline void ADD_Vx_Vy(Chip8 *chip8, uint16_t opcode)
{
    uint8_t vx = decode_vx(opcode);
    uint8_t vy = decode_vy(opcode);
    if(((uint16_t)chip8->V[vx] + (uint16_t)chip8->V[vy]) > 255) 
        chip8->V[0xF] = 1; // overflow
    else chip8->V[0xF] = 0;
    chip8->V[vx] += chip8->V[vy];
    return;
}

static inline void SUB_Vx_Vy(Chip8 *chip8, uint16_t opcode)
{
    uint8_t vx = decode_vx(opcode);
    uint8_t vy = decode_vy(opcode);
    if(chip8->V[vx] > chip8->V[vy]) chip8->V[0xF] = 1; // ?
    else chip8->V[0xF] = 0; // ?
    chip8->V[vx] -= chip8->V[vy];
    return;
}

static inline void SHR_Vx_Vy(Chip8 *chip8, uint16_t opcode)
{   
    uint8_t vx = decode_vx(opcode);
    chip8->V[0xF] = 1 & chip8->V[vx];
    chip8->V[vx] >>= 1;
    return;
}

static inline void SUBN_Vx_Vy(Chip8 *chip8, uint16_t opcode)
{
    uint8_t vx = decode_vx(opcode);
    uint8_t vy = decode_vy(opcode);
    if(chip8->V[vy] > chip8->V[vx]) chip8->V[0xF] = 1; // ?
    else chip8->V[0xF] = 0; // ?
    chip8->V[vx] = chip8->V[vy] - chip8->V[vx];
    return;
}

static inline void SHL_Vx_Vy(Chip8 *chip8, uint16_t opcode)
{   uint8_t vx = decode_vx(opcode);
    chip8->V[0xF] = 1 & chip8->V[vx];
    chip8->V[vx] <<= 1;
    // TODO overflow? no?
    return;
}

static inline void LD_I_Addr(Chip8 *chip8, uint16_t opcode)
{
    chip8->I = decode_nnn(opcode);
    return;
}

static inline void RND_Vx_Byte(Chip8 *chip8, uint16_t opcode)
{
    chip8->V[decode_vx(opcode)] = (uint8_t)(rand() % 256) & decode_nn(opcode);
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

static inline void SKP_Vx(Chip8 *chip8, uint16_t opcode)
{   
    if (chip8->key[chip8->V[decode_vx(opcode)]]) chip8->PC += 2;
    return;
}
static inline void SKNP_Vx(Chip8 *chip8, uint16_t opcode)
{   
    if ( ! chip8->key[chip8->V[decode_vx(opcode)]]) chip8->PC += 2;
    return;
}

static inline void LD_Vx_DT(Chip8 *chip8, uint16_t opcode)
{
    chip8->V[decode_vx(opcode)] = chip8->DT;
    return;
}

static inline void LD_ST_Vx(Chip8 *chip8, uint16_t opcode)
{
    chip8->ST = chip8->V[decode_vx(opcode)];
    return;
}

static inline void LD_DT_Vx(Chip8 *chip8, uint16_t opcode)
{
    chip8->DT = chip8->V[decode_vx(opcode)];
    return;
}

static inline void LD_ArrayI_Vx(Chip8 *chip8, uint16_t opcode)
{
    memcpy(chip8->mem + chip8->I, chip8->V, decode_vx(opcode) + 1);
    return;
}

static inline void LD_ArrayVx_I(Chip8 *chip8, uint16_t opcode)
{
    memcpy(chip8->V, chip8->mem + chip8->I, decode_vx(opcode) + 1);
    return;
}

static inline void LD_I_FontVx(Chip8 *chip8, uint16_t opcode)
{
    chip8->I = FONT_START + 5*chip8->V[decode_vx(opcode)];
    return;
}

static inline void LD_I_BCDVx(Chip8 *chip8, uint16_t opcode)
{
    uint16_t I = chip8->I;
    uint8_t vx = chip8->V[decode_vx(opcode)];
    chip8->mem[I] = vx / 100;
    chip8->mem[I+1] = (vx / 10) % 10;
    chip8->mem[I+2] = (vx % 100) % 10;
    return;
}

static inline void ADD_I_Vx(Chip8 *chip8, uint16_t opcode)
{
    chip8->I += chip8->V[decode_vx(opcode)];
    // TODO possibly put a switch on this for carry
    return;
}

static inline void LD_Vx_Key(Chip8 *chip8, uint16_t opcode)
{
    for(int i = 0; i < 16; ++i)
        if (chip8->key[i]){
            chip8->V[decode_vx(opcode)] = i;
            return;
        }
    chip8->PC -= 2; // block until we have a keypress
    return;
}

static inline void JP_V0_Addr(Chip8 *chip8, uint16_t opcode)
{
    chip8->PC = decode_nnn(opcode) + chip8->V[0];
    return;
}
