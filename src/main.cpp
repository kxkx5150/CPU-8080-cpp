#include <cstdint>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <cstdio>
#include <time.h>

#include "PC.h"
#include "cpu.h"
#include "screen.h"


int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window  *win   = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, ZOOM * WIDTH,
                                          ZOOM * HEIGHT, SDL_WINDOW_OPENGL);
    SDL_Surface *wsurf = SDL_GetWindowSurface(win);
    SDL_Surface *surf  = SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0);


    PC *pc = new PC(win, surf, wsurf);
    pc->load_rom("invaders.rom");

    uint32_t last_tic = SDL_GetTicks();
    while (1) {
        if ((SDL_GetTicks() - last_tic) >= 1000.0 / 60.0) {
            last_tic = SDL_GetTicks();
            pc->tick();
        }
    }
    return 0;
}
