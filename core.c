#include <stdio.h>
#include <SDL.h>
#include "chip8.h"

int main(int argc, char **argv)
{
    SDL_Window *window = NULL;
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        printf("SDL: %s", SDL_GetError());
    else{
        window = SDL_CreateWindow("CChip8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
        if(window == NULL)
            printf("SDL: %s", SDL_GetError());
        else{
            SDL_UpdateWindowSurface(window);
            SDL_Delay(50);
        }
    }
    // set up input

    Chip8 chip8;

    reset(chip8);
    load_game(chip8, "pong");
    SDL_Event(e);

    while(1)
    {
        SDL_PollEvent(&e);
	if (e.type == SDL_QUIT){
            SDL_DestroyWindow(window);
            SDL_Quit();
            break;
        }
        cpu_cycle(chip8);
    }
    
    return 0;
}
