#ifndef _H_SCREEN_
#define _H_SCREEN_

#include <SDL2/SDL.h>
#include "mem.h"
#include <cstdint>

#define HEIGHT 256
#define WIDTH  224
#define ZOOM   2


class Screen {
  public:
    Mem *mem;

    SDL_Window  *win;
    SDL_Surface *surf;
    SDL_Surface *winsurf;

  public:
    Screen(Mem *_mem, SDL_Window *_win, SDL_Surface *_surf, SDL_Surface *_winsurf);
    ~Screen();

    void init();
    void draw();
};

#endif
