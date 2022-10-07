#ifndef _H_CPU_
#define _H_CPU_
#include <cstdint>
#include <stdint.h>
#include "mem.h"


class CPU {
  public:
    Mem *mem = nullptr;

    uint16_t pc;    // Program counter
    uint16_t sp;    // Stack pointer

    union
    {
        struct
        {
            uint8_t c;    // general purpose registers
            uint8_t b;
            uint8_t e;
            uint8_t d;
            uint8_t l;
            uint8_t h;
            uint8_t z;    // temporary registers
            uint8_t w;
            union
            {
                uint8_t f;    // Status Register
                struct
                {
                    uint8_t cy : 1;    // Carry
                    uint8_t : 1;
                    uint8_t p : 1;    // Parity
                    uint8_t : 1;
                    uint8_t ac : 1;    // Auxiliary Carry
                    uint8_t i : 1;     // Interrupt
                    uint8_t z : 1;     // Zero
                    uint8_t s : 1;     // Sign
                } flags;
            };
            uint8_t a;    // Accumulator
        };
        struct
        {
            uint16_t bc;
            uint16_t de;
            uint16_t hl;
            uint16_t wz;
            uint16_t af;
        };
    };

    uint8_t ports[9];    // Ports

  public:
    CPU(Mem *_mem);

    void read_bytes_to_wz();
    void read_byte_to_z();
    void handle_flags(uint32_t result, size_t size, int flags);

    void     push(uint16_t value);
    uint16_t pop();

    void run(uint64_t cycles);
    void init(Mem *_mem);
    int  exec_instruction(uint8_t ir);
    void generate_irq(uint16_t addr);

  private:
    int MOV(uint8_t *dest, const uint8_t *src);
    int MOV_from_mem(uint8_t *src);
    int MOV_to_mem(const uint8_t *src);
    int MVI(uint8_t *dest);
    int MVI_to_mem();
    int LXI(uint16_t *dest);
    int LDA();
    int STA();
    int LHLD();
    int SHLD();
    int LDAX_B();
    int LDAX_D();
    int STAX_B();
    int STAX_D();
    int XCHG();
    int ADD(uint8_t *r);
    int ADD_M();
    int ADI();
    int ADC(uint8_t *r);
    int SUB(uint8_t *r);
    int SUI();
    int SBI();
    int INR(uint8_t *r);
    int INR_M();
    int DCR(uint8_t *r);
    int DCR_M();
    int INX(uint16_t *rp);
    int DCX(uint16_t *rp);
    int DAD(uint16_t *rp);
    int DAA();
    int ANA(uint8_t *r);
    int ANA_M();
    int ANI();
    int XRA(uint8_t *r);
    int ORA(uint8_t *r);
    int ORA_M();
    int ORI();
    int CMP(uint8_t *r);
    int CMP_M();
    int CPI();
    int RLC();
    int RRC();
    int RAR();
    int CMA();
    int STC();
    int JMP();
    int JZ();
    int JNZ();
    int JC();
    int JNC();
    int JM();
    int CALL();
    int CZ();
    int CNZ();
    int CNC();
    int RET();
    int RZ();
    int RNZ();
    int RC();
    int RNC();
    int PCHL();
    int PUSH_B();
    int PUSH_D();
    int PUSH_H();
    int PUSH_PSW();
    int POP_B();
    int POP_D();
    int POP_H();
    int POP_PSW();
    int XTHL();
    int IN();
    int OUT();
    int EI();
    int NOP();
};

#endif
