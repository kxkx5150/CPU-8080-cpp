#include "screen.h"


Screen::Screen(Mem *_mem, SDL_Window *_win, SDL_Surface *_surf, SDL_Surface *_winsurf)
{
    win     = _win;
    surf    = _surf;
    winsurf = _winsurf;

    mem = _mem;
}
Screen::~Screen()
{
}
void Screen::init()
{
}
void Screen::draw()
{
    uint32_t *pix = (uint32_t *)surf->pixels;
    int       i   = 0x2400;

    for (int col = 0; col < WIDTH; col++) {
        for (int row = HEIGHT; row > 0; row -= 8) {
            for (int j = 0; j < 8; j++) {
                int idx = (row - j) * WIDTH + col;
                if (mem->get(i) & 1 << j) {
                    pix[idx] = 0xFFFFFF;
                } else {
                    pix[idx] = 0x000000;
                }
            }
            i++;
        }
    }

    SDL_BlitScaled(surf, NULL, winsurf, NULL);
    if (SDL_UpdateWindowSurface(win)) {
        puts(SDL_GetError());
    }
}
