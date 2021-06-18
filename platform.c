#include "chip8.h"

#include <SDL.h>

#define INITIAL_AUDIO_CACHE 60 // seconds
#define AUDIO_CACHE 5 // seconds

static inline void play_audio();
static inline void pause_audio();
static inline void queue_audio();
static void create_sinewave(const int sample_len);
static void draw_frame(Chip8 *chip8);
static int get_input(Chip8 *chip8);

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

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *texture = NULL;
SDL_AudioDeviceID audio_dev;
SDL_AudioSpec audio_spec;
SDL_Event e;

uint32_t *pixels = NULL;

int init_platform(Options *options)
{

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0){
        printf("SDL: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow("CChip8", SDL_WINDOWPOS_CENTERED, \
                                        SDL_WINDOWPOS_CENTERED, \
                                        64 * options->scale, \
                                        32 * options->scale, \
                                        SDL_WINDOW_SHOWN);
    if (window == NULL) printf("SDL: %s", SDL_GetError());
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) printf("SDL: %s", SDL_GetError());
    texture = SDL_CreateTexture(renderer, \
                SDL_PIXELFORMAT_ARGB8888, \
                SDL_TEXTUREACCESS_STATIC, 64, 32);
    if (texture == NULL) printf("SDL: %s", SDL_GetError());

    audio_dev = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
    if ( ! audio_dev) printf("SDL: %s", SDL_GetError());

    pixels = calloc(GFXSIZE, sizeof(uint32_t));
    if (pixels == NULL){
        fprintf(stderr, "FATAL: Unable to alloc pixel buffer!\n");
        return EXIT_FAILURE;
    }

    SDL_zero(audio_spec);
    audio_spec.freq = 44100;
    audio_spec.format = AUDIO_S16SYS;
    audio_spec.channels = 1;
    audio_spec.samples = 1024;

    create_sinewave(INITIAL_AUDIO_CACHE);

    return 0;
}

int update_platform(Chip8 *chip8)
{
    if (chip8->draw_cycle)
        draw_frame(chip8); // opcode updated screen

    if (SDL_GetQueuedAudioSize(audio_dev) < audio_spec.freq)
        queue_audio(); // buffer more audio

    if (chip8->ST > 0)
        play_audio();
    else
        pause_audio();

    return get_input(chip8);   
}

static void draw_frame(Chip8 *chip8)
{
    for (int i = 0; i < chip8->gfxsize; i++){
        if (chip8->gfx[i]) pixels[i] = 0xFFFFFFFF;
        else pixels[i] = 0xFF000000;
    }
    SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

static int get_input(Chip8 *chip8)
{
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT){
            SDL_DestroyTexture(texture);
            SDL_DestroyRenderer(renderer);
            SDL_CloseAudioDevice(audio_dev);
            SDL_DestroyWindow(window);
            free(pixels);
            SDL_Quit();
            return -1;
        }

        if (e.type == SDL_KEYDOWN)
            for(int i = 0; i < 16; ++i)
                if (keymap[i] == e.key.keysym.scancode)
                    chip8->key[i] = 1;

        if (e.type == SDL_KEYUP)
            for(int i = 0; i < 16; ++i)
                if (keymap[i] == e.key.keysym.scancode)
                        chip8->key[i] = 0;
    }
    return 0;
}

static void create_sinewave(const int sample_len)
{
    float x = 0.0;
    for (int i = 0; i < audio_spec.freq * sample_len; i++) {
        x += 0.01;
        int sinwave_sample = sin(x * 4) * 5000;
        SDL_QueueAudio(audio_dev, &sinwave_sample, 2);
    }
}

static inline void queue_audio()
{
    create_sinewave(AUDIO_CACHE);
}

static inline void play_audio()
{
    SDL_PauseAudioDevice(audio_dev, 0);
}

static inline void pause_audio()
{
    SDL_PauseAudioDevice(audio_dev, 1);
}
