#ifndef _H_PC
#define _H_PC
#include "cpu.h"
#include "mem.h"
#include "screen.h"


class PC {
  public:
    CPU    *cpu    = nullptr;
    Mem    *mem    = nullptr;
    Screen *screen = nullptr;


  public:
    PC(SDL_Window *win, SDL_Surface *surf, SDL_Surface *winsurf);
    ~PC();

    void init();
    void load_rom(std::string path);
    void tick();
    void handle_input();

  private:
};
#endif
