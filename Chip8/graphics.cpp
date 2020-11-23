#include "graphics.h"
#include <iostream>
#include "emulatordata.h"

using namespace std;

SDL_Window* create_window(
    int width,
    int height,
    const char* title,
    Uint32 flags
)
{
    // retutns zero on success else non-zero 
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        std::cout << "error initializing SDL: " << SDL_GetError() << "\n";
    }

    SDL_Window* win = SDL_CreateWindow(title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height, flags);

    return win;
}

SDL_Renderer* init_renderer(SDL_Window* window)
{
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;

    return SDL_CreateRenderer(window, -1, render_flags);
}

void set_pixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
    Uint32* pixels = (Uint32*)surface->pixels;
    pixels[(y * surface->pitch) + x] = pixel;
}

void render()
{
    // clears the screen 
    SDL_RenderClear(renderer);
    uint8_t* pixels;
    int pitch;
    SDL_LockTexture(tex, NULL, (void**) &pixels, &pitch);
    for (size_t y = 0; y < 32; y++)
    {
        Uint32* pixel = (Uint32*)(pixels + pitch * y);
        for (size_t x = 0; x < 64; x++)
        {
            uint64_t row = screen_buffer[y];
            // 0b0100
            int pixel_enabled = (row >> (63 - x)) & 1;
            *pixel = SDL_MapRGBA(surface->format, pixel_enabled * 100, 0, pixel_enabled * 255, 255);

            pixel++;
        }
    }
    SDL_UnlockTexture(tex);
    SDL_RenderClear(renderer);
    SDL_Rect dst_rect = { 0, 0, 64 * 100, 32 * 100 };
    SDL_RenderCopy(renderer, tex, NULL, &dst_rect);
    // triggers the double buffers 
    // for multiple rendering 
    SDL_RenderPresent(renderer);

    // calculates to 60 fps 
    SDL_Delay(1000 / 60);
}
