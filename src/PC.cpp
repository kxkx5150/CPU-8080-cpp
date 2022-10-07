#include "PC.h"
#include "cpu.h"
#include <SDL2/SDL_keycode.h>

#define CYCLES_PER_MS  2000
#define CYCLES_PER_TIC (CYCLES_PER_MS * 1000.0 / 60.0)


PC::PC(SDL_Window *_win, SDL_Surface *_surf, SDL_Surface *_winsurf)
{
    mem    = new Mem();
    cpu    = new CPU(mem);
    screen = new Screen(mem, _win, _surf, _winsurf);
    init();
}
PC::~PC()
{
    delete cpu;
    delete mem;
    delete screen;
}
void PC::init()
{
    mem->init();
    screen->init();
    cpu->init(mem);
}
void PC::load_rom(std::string path)
{
    mem->load_rom(path);
}
void PC::tick()
{
    cpu->run(CYCLES_PER_TIC);
    screen->draw();
    handle_input();
    if (cpu->flags.i) {
        cpu->generate_irq(0x10);
    }
}
void PC::handle_input()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
            case SDL_KEYDOWN:
                switch (ev.key.keysym.sym) {
                    case 'c':
                        cpu->ports[1] |= 1;
                        break;
                    case 's':
                        cpu->ports[1] |= 1 << 2;
                        break;
                    case SDLK_SPACE:
                        cpu->ports[1] |= 1 << 4;
                        break;
                    case SDLK_LEFT:
                        cpu->ports[1] |= 1 << 5;
                        break;
                    case SDLK_RIGHT:
                        cpu->ports[1] |= 1 << 6;
                        break;
                    case SDLK_a:
                        cpu->ports[2] |= 1 << 5;
                        break;
                    case SDLK_d:
                        cpu->ports[2] |= 1 << 6;
                        break;
                    case SDLK_w:
                        cpu->ports[1] |= 1 << 1;
                        break;
                    case SDLK_UP:
                        cpu->ports[2] |= 1 << 4;
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (ev.key.keysym.sym) {
                    case 'c':
                        cpu->ports[1] &= ~1;
                        break;
                    case 's':
                        cpu->ports[1] &= ~(1 << 2);
                        break;
                    case SDLK_SPACE:
                        cpu->ports[1] &= ~(1 << 4);
                        break;
                    case SDLK_LEFT:
                        cpu->ports[1] &= ~(1 << 5);
                        break;
                    case SDLK_RIGHT:
                        cpu->ports[1] &= ~(1 << 6);
                        break;
                    case SDLK_a:
                        cpu->ports[2] &= ~(1 << 5);
                        break;
                    case SDLK_d:
                        cpu->ports[2] &= ~(1 << 6);
                        break;
                    case SDLK_w:
                        cpu->ports[1] &= ~(1 << 1);
                        break;
                    case SDLK_UP:
                        cpu->ports[2] &= ~(1 << 4);
                        break;
                    case 'q':
                        exit(0);
                        break;
                }
                break;
            case SDL_QUIT:
                exit(0);
                break;
        }
    }
}