#include "chip8.h"

#include <getopt.h>

#define SPEED 600 // hz
#define SCALE 20 // 64x32 multiplier

static Options *parse_options(int argc, char *const argv[]);
static inline int fatal_error(const char *msg);

int main(int argc, char *const argv[])
{
    Options *options = parse_options(argc, argv);
    if (options == NULL) return fatal_error("Failed to alloc options struct.");
    if (options->speed < 60) return fatal_error("CPU speed must be at least 60hz");
    if (options->scale < 1) return fatal_error("Window scale must be at least 1.");
    if (options->rom == NULL) return fatal_error("No ROM provided.");

    init_platform(options);

    Chip8 chip8 = { MEMSIZE, GFXSIZE, STACKSIZE };

    reset(&chip8);
    int lr = load_rom(&chip8, options->rom);
    if (lr != 0) return fatal_error("Could not load ROM at the given location.");

    uint64_t ctr = 0; // shouldn't overflow at intended range of speeds

    while(ctr += 1)
    {
        cycle(&chip8);
        if ( ! (ctr % (SPEED/60))) dec_timers(&chip8); // approximate timing
        if (update_platform(&chip8) < 0) break;
        nanosleep((const struct timespec[]){{0, 1000000000L/options->speed}}, NULL);
    }

    free(options);
    
    return 0;
}

static Options *parse_options(int argc, char *const argv[])
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

static inline int fatal_error(const char *msg)
{
    fprintf(stderr, "%s Exiting...\n", msg);
    return EXIT_FAILURE;
}