#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "chip8.h"

#define MEMSIZE 4096
#define GFXSIZE 2048
#define STACKSIZE 16
#define SCALE 20
#define SPEED 600 // hz

static int keymap[16] = {
    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_4,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_W,
    SDL_SCANCODE_E,
    SDL_SCANCODE_R,
    SDL_SCANCODE_A,
    SDL_SCANCODE_S,
    SDL_SCANCODE_D,
    SDL_SCANCODE_F,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_X,
    SDL_SCANCODE_C,
    SDL_SCANCODE_V,
};

int main(int argc, char *const argv[])
{
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    SDL_Event e;
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        printf("SDL: %s", SDL_GetError());
    else{
        window = SDL_CreateWindow("CChip8", SDL_WINDOWPOS_CENTERED, \
                                            SDL_WINDOWPOS_CENTERED, \
                                            64 * SCALE, 32 * SCALE, \
                                            SDL_WINDOW_SHOWN);
        if(window == NULL) printf("SDL: %s", SDL_GetError());
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if(renderer == NULL) printf("SDL: %s", SDL_GetError());
        texture = SDL_CreateTexture(renderer, \
                    SDL_PIXELFORMAT_ARGB8888, \
                    SDL_TEXTUREACCESS_STATIC, 64, 32);
        if(texture == NULL) printf("SDL: %s", SDL_GetError());
    }
    // set up input

    Chip8 chip8 = { MEMSIZE, GFXSIZE, STACKSIZE };

    reset(&chip8);
    if (argc < 2) {
        fprintf(stderr, "No ROM provided. Exiting...\n");
        return EXIT_FAILURE;
    }
    load_rom(&chip8, argv[1]);
    uint32_t pixels[GFXSIZE];
    uint64_t ctr = 0; // shouldn't overflow at intended range of speeds

    while(ctr += 1)
    {
        cycle(&chip8);
        if (chip8.draw_cycle){ // opcode updated screen
            for (int i = 0; i < GFXSIZE; i++){
                if (chip8.gfx[i]) pixels[i] = 0xFFFFFFFF;
                else pixels[i] = 0xFF000000;
            }
            SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }
        if (chip8.ST > 0){
            // TODO audio.play()
        }
        else // audio.stop() 

        if ( ! (ctr % (SPEED/60))) dec_timers(&chip8); // approximate timing
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT){
                SDL_DestroyTexture(texture);
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                goto quit;
            }
            if (e.type == SDL_KEYDOWN){
                for(int i = 0; i < 16; ++i){
                    if (keymap[i] == e.key.keysym.scancode)
                        chip8.key[i] = 1;;
                }

	        }
            if (e.type == SDL_KEYUP){
                for(int i = 0; i < 16; ++i){
                    if (keymap[i] == e.key.keysym.scancode)
                        chip8.key[i] = 0;
                }

	        }
        }
        nanosleep((const struct timespec[]){{0, 1000000000L/(long)SPEED}}, NULL);
    }

    quit:
    
    return 0;
}
