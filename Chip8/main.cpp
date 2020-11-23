//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>
#include "graphics.h"
#include "emulatorloop.h"
#include <iostream>
#include "emulatordata.h"
#include "emulator.h"

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 320;

// variables
SDL_Window* win;

void start();

int main(int argc, char* args[])
{
    read_rom_from_file("./test-roms/Space Invaders [David Winter].ch8");
    start();
    
    return 0;
}

void start()
{
    win = create_window(
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        "CHIP-8",
        0
    );

    renderer = init_renderer(win);
    surface = SDL_GetWindowSurface(win);

    // clears main-memory 
    SDL_FreeSurface(surface);

    //SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);
    tex = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );

    for (size_t i = 0; i < 32; i++)
    {
        screen_buffer[i] = 0;
    }

    bool close = false;

    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 64; j++)
        {
            SDL_Rect rect = { j, i, 100, 100 };
            //std::cout << (int)(i * 64 + j) << std::endl;
        }
    }

    // emulator loop
    while (!close)
    {
        handle_events(&close);
        emulator_tick();
    }

    // destroy texture 
    SDL_DestroyTexture(tex);

    // destroy renderer 
    SDL_DestroyRenderer(renderer);

    // destroy window 
    SDL_DestroyWindow(win);
}