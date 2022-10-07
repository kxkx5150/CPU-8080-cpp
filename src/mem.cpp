#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include "mem.h"
#include "rom.h"

Mem::Mem()
{
}
Mem::~Mem()
{
}
uint8_t Mem::get(uint16_t addr)
{
    return ram[addr];
}
void Mem::set(uint16_t addr, uint8_t data)
{
    ram[addr] = data;
}
void Mem::init()
{
    reset();
}
void Mem::reset()
{
    for (int i = 0; i < MEMLEN; ++i)
        ram[i] = 0;
}
void Mem::load_rom(std::string path)
{
    if (path == "invaders.rom") {
        for (int i = 0; i < 8192; ++i)
            ram[i] = invaders[i];
    } else {
        FILE *f = fopen(path.c_str(), "rb");
        fseek(f, 0, SEEK_END);
        int size = ftell(f);
        fseek(f, 0, SEEK_SET);
        auto _ = fread(ram, size, 1, f);
    }
}
