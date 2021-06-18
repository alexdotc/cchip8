#include "chip8.h"

#include <getopt.h>

#define SPEED 600 // hz
#define SCALE 20 // 64x32 multiplier

Options *parse_options(int argc, char *const argv[]);

int main(int argc, char *const argv[])
{
    Options *options = parse_options(argc, argv);
    if (options == NULL) {
        fprintf(stderr, "Failed to alloc options struct. Exiting...\n");
        return EXIT_FAILURE;
    }
    if (options->speed < 60){
        fprintf(stderr, "CPU speed must be at least 60hz. Exiting...\n");
        return EXIT_FAILURE;
    }
    if (options->rom == NULL) {
        fprintf(stderr, "No ROM provided. Exiting...\n");
        return EXIT_FAILURE;
    }

    init_platform(options);

    Chip8 chip8 = { MEMSIZE, GFXSIZE, STACKSIZE };

    reset(&chip8);
    load_rom(&chip8, options->rom);
    uint64_t ctr = 0; // shouldn't overflow at intended range of speeds

    while(ctr += 1)
    {
        cycle(&chip8);
        if ( ! (ctr % (SPEED/60))) dec_timers(&chip8); // approximate timing
        if (update_platform(&chip8) < 0) goto quit;
        nanosleep((const struct timespec[]){{0, 1000000000L/options->speed}}, NULL);
    }

    quit:
    free(options);
    
    return 0;
}

Options *parse_options(int argc, char *const argv[])
{
    int opt;
    Options *options = malloc(sizeof(Options));
    if (options == NULL) return options;
    options->speed = SPEED;
    options->scale = SCALE;
    options->rom = NULL;

    while((opt = getopt(argc, argv, "x:s:")) != -1)
        switch(opt){
            case 'x': options->scale = strtol(optarg, NULL, 0); break;
            case 's': options->speed = strtol(optarg, NULL, 0); break;
            default: ;
        }

    if (optind < argc) options->rom = argv[optind++];

    return options;
}