#ifndef _H_MEM_
#define _H_MEM_
#include <cstddef>
#include <cstdint>
#include <vector>
#include <string>

#define MEMLEN 0x10000


class Mem {
  private:
    uint8_t ram[MEMLEN] = {0};

  public:
    Mem();
    ~Mem();

    uint8_t get(uint16_t addr);
    void    set(uint16_t addr, uint8_t data);

    void init();
    void reset();

    void load_rom(std::string path);
};
#endif
