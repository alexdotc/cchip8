#include "chip8.h"
#include "platform.h"

#define SPEED 600 // hz

int main(int argc, char *const argv[])
{
    init_platform();

    Chip8 chip8 = { MEMSIZE, GFXSIZE, STACKSIZE };

    reset(&chip8);
    if (argc < 2) {
        fprintf(stderr, "No ROM provided. Exiting...\n");
        return EXIT_FAILURE;
    }
    load_rom(&chip8, argv[1]);
    uint64_t ctr = 0; // shouldn't overflow at intended range of speeds

    while(ctr += 1)
    {
        cycle(&chip8);
        if ( ! (ctr % (SPEED/60))) dec_timers(&chip8); // approximate timing
        if (update_platform(&chip8) < 0) goto quit;
        nanosleep((const struct timespec[]){{0, 1000000000L/(long)SPEED}}, NULL);
    }

    quit:
    
    return 0;
}
