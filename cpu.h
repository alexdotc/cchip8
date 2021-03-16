typedef struct Chip8CPU
{
    unsigned short PC; // program counter
    unsigned short I; // address register
    unsigned short SP; // stack pointer
    unsigned char stack[16];
    unsigned char V[16]; // data registers

} Chip8CPU;

void cycle(Chip8CPU cpu);
