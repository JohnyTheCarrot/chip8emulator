#pragma once
#include <SDL.h>

SDL_Window* create_window(
    int width,
    int height,
    const char* title,
    Uint32 flags
);

SDL_Renderer* init_renderer(
    SDL_Window* window
);

void set_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel);

void render();
