# cchip8
A CHIP-8 interpreter in C. SDL2 as a graphics frontend. All the basic functionality is there with a handful of games tested.

## System Requirements

- Linux x64 with libc from the last 20ish years
- SDL2 library and devel headers

## Instructions to Build and Run
    make
    ./chip8.out [OPTIONS]... ROM

## Options

- `-x` to set the window scale to a 64x32 multiple (default: 20)
- `-s` to set the CPU frequency in hz (default: 600)

## Credits

[Cowgod's CHIP-8 spec/reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)  
[Intro to Emulators](https://multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/)
