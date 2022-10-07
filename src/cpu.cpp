#include <cstdint>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cpu.h"

#define F_ALL (f_z | f_s | f_p | f_cy | f_ac)
#define F_ZSP (f_z | f_s | f_p)

typedef enum CF
{
    f_z  = 1,
    f_s  = 1 << 1,
    f_p  = 1 << 2,
    f_cy = 1 << 3,
    f_ac = 1 << 4
} flag;


CPU::CPU(Mem *_mem)
{
    mem = _mem;
}
void CPU::run(uint64_t cycles)
{
    int  i   = 0;
    bool flg = false;

    while (i < cycles) {
        if (cycles / 2 < i && !flg && flags.i) {
            flg = true;
            generate_irq(0x08);
        }

        uint8_t ir = mem->get(pc++);

        uint16_t shift_register;
        uint64_t shift_amount;

        if (ir == 0xd3) {
            if (mem->get(pc) == 2) {
                shift_amount = a;
            } else if (mem->get(pc) == 4) {
                shift_register = (a << 8) | (shift_register >> 8);
            }
        } else if (ir == 0xdb) {
            if (mem->get(pc) == 3) {
                a = shift_register >> (8 - shift_amount);
            }
        }

        i += exec_instruction(ir);
    }
}
void CPU::init(Mem *_mem)
{
    mem = _mem;
}

//

int CPU::MOV(uint8_t *dest, const uint8_t *src)
{
    *dest = *src;
    return 5;
}
int CPU::MOV_from_mem(uint8_t *src)
{
    *src = mem->get(hl);
    return 7;
}
int CPU::MOV_to_mem(const uint8_t *src)
{
    mem->set(hl, *src);
    return 7;
}
int CPU::MVI(uint8_t *dest)
{
    read_byte_to_z();
    *dest = z;
    return 7;
}
int CPU::MVI_to_mem()
{
    read_byte_to_z();
    mem->set(hl, z);
    return 10;
}
int CPU::LXI(uint16_t *dest)
{
    read_bytes_to_wz();
    *dest = wz;
    return 10;
}
int CPU::LDA()
{
    read_bytes_to_wz();
    a = mem->get(wz);
    return 13;
}
int CPU::STA()
{
    read_bytes_to_wz();
    mem->set(wz, a);
    return 13;
}
int CPU::LHLD()
{
    read_bytes_to_wz();
    h = mem->get(wz + 1);
    l = mem->get(wz);
    return 16;
}
int CPU::SHLD()
{
    read_bytes_to_wz();
    mem->set(wz + 1, h);
    mem->set(wz, l);
    return 16;
}
int CPU::LDAX_B()
{
    a = mem->get(bc);
    return 7;
}
int CPU::LDAX_D()
{
    a = mem->get(de);
    return 7;
}
int CPU::STAX_B()
{
    mem->set(bc, a);
    return 7;
}
int CPU::STAX_D()
{
    mem->set(de, a);
    return 7;
}
int CPU::XCHG()
{
    hl ^= de;
    de ^= hl;
    hl ^= de;
    return 4;
}
int CPU::ADD(uint8_t *r)
{
    uint16_t result = a + *r;
    a               = result;
    handle_flags(result, 8, F_ALL);
    return 4;
}
int CPU::ADD_M()
{
    uint16_t result = a + mem->get(hl);
    a               = result;
    handle_flags(result, 8, F_ALL);
    return 7;
}
int CPU::ADI()
{
    read_byte_to_z();
    uint16_t result = a + z;
    a               = result;
    handle_flags(result, 8, F_ALL);
    return 7;
}
int CPU::ADC(uint8_t *r)
{
    uint16_t result = a + *r + flags.cy;
    a               = result;
    handle_flags(result, 8, F_ALL);
    return 4;
}
int CPU::SUB(uint8_t *r)
{
    uint16_t result = a - *r;
    a               = result;
    handle_flags(result, 8, F_ALL);
    return 4;
}
int CPU::SUI()
{
    read_byte_to_z();
    uint16_t result = a - z;
    a               = result;
    handle_flags(result, 8, F_ALL);
    return 7;
}
int CPU::SBI()
{
    read_byte_to_z();
    uint16_t result = a - z - flags.cy;
    a               = result;
    handle_flags(result, 8, F_ALL);
    return 7;
}
int CPU::INR(uint8_t *r)
{
    uint16_t result = *r + 1;
    *r              = result;
    handle_flags(result, 8, F_ZSP | f_ac);
    return 5;
}
int CPU::INR_M()
{
    uint32_t result = mem->get(hl) + 1;
    mem->set(hl, result);
    handle_flags(result, 16, F_ZSP | f_ac);
    return 10;
}
int CPU::DCR(uint8_t *r)
{
    uint16_t result = *r - 1;
    *r              = result;
    handle_flags(result, 8, F_ZSP | f_ac);
    return 5;
}
int CPU::DCR_M()
{
    uint32_t result = mem->get(hl) - 1;
    mem->set(hl, result);
    handle_flags(result, 16, F_ZSP | f_ac);
    return 10;
}
int CPU::INX(uint16_t *rp)
{
    (*rp)++;
    return 5;
}
int CPU::DCX(uint16_t *rp)
{
    (*rp)--;
    return 5;
}
int CPU::DAD(uint16_t *rp)
{
    uint32_t result = hl + *rp;
    hl              = result;
    handle_flags(result, 16, f_cy);
    return 10;
}
int CPU::DAA()
{
    return 4;
}
int CPU::ANA(uint8_t *r)
{
    uint8_t result = a & *r;
    a              = result;
    handle_flags(result, 8, F_ZSP | f_ac);
    flags.cy = 0;
    return 4;
}
int CPU::ANA_M()
{
    uint8_t result = a & mem->get(hl);
    a              = result;
    handle_flags(result, 8, F_ZSP);
    flags.cy = flags.ac = 0;
    return 7;
}
int CPU::ANI()
{
    read_byte_to_z();
    uint8_t result = a & z;
    a              = result;
    handle_flags(result, 8, F_ZSP);
    flags.cy = flags.ac = 0;
    return 7;
}
int CPU::XRA(uint8_t *r)
{
    uint8_t result = a ^ *r;
    a              = result;
    handle_flags(result, 8, F_ZSP);
    flags.cy = flags.ac = 0;
    return 4;
}
int CPU::ORA(uint8_t *r)
{
    uint8_t result = a | *r;
    a              = result;
    handle_flags(result, 8, F_ZSP);
    flags.cy = flags.ac = 0;
    return 4;
}
int CPU::ORA_M()
{
    uint8_t result = a | mem->get(hl);
    a              = result;
    handle_flags(result, 8, F_ZSP);
    flags.cy = flags.ac = 0;
    return 7;
}
int CPU::ORI()
{
    read_byte_to_z();
    uint8_t result = a | z;
    a              = result;
    handle_flags(result, 8, F_ZSP);
    flags.cy = flags.ac = 0;
    return 7;
}
int CPU::CMP(uint8_t *r)
{
    uint16_t result = a - *r;
    handle_flags(result, 8, F_ALL);
    return 4;
}
int CPU::CMP_M()
{
    uint16_t result = a - mem->get(hl);
    handle_flags(result, 8, F_ALL);
    return 7;
}
int CPU::CPI()
{
    read_byte_to_z();
    uint16_t result = a - z;
    handle_flags(result, 8, F_ALL);
    return 7;
}
int CPU::RLC()
{
    uint8_t temp = a;
    a            = (temp << 1) | (temp & 0x80) >> 7;
    flags.cy     = (temp & 0x80) >> 7;
    return 4;
}
int CPU::RRC()
{
    uint8_t temp = a;
    a            = ((temp & 1) << 7) | (temp >> 1);
    flags.cy     = (temp & 1);
    return 4;
}
int CPU::RAR()
{
    uint8_t temp = a;
    a            = (flags.cy << 7) | (temp >> 1);
    flags.cy     = (temp & 1);
    return 4;
}
int CPU::CMA()
{
    a = ~a;
    return 4;
}
int CPU::STC()
{
    flags.cy = 1;
    return 4;
}
int CPU::JMP()
{
    pc = wz;
    return 10;
}
int CPU::JZ()
{
    if (flags.z) {
        JMP();
    }
    return 10;
}
int CPU::JNZ()
{
    if (!flags.z) {
        JMP();
    }
    return 10;
}
int CPU::JC()
{
    if (flags.cy) {
        JMP();
    }
    return 10;
}
int CPU::JNC()
{
    if (!flags.cy) {
        JMP();
    }
    return 10;
}
int CPU::JM()
{
    if (flags.s) {
        JMP();
    }
    return 10;
}
int CPU::CALL()
{
    push(pc);
    pc = wz;
    return 17;
}
int CPU::CZ()
{
    if (flags.z) {
        CALL();
        return 17;
    } else {
        return 11;
    }
}
int CPU::CNZ()
{
    if (!flags.z) {
        CALL();
        return 17;
    } else {
        return 11;
    }
}
int CPU::CNC()
{
    if (!flags.cy) {
        CALL();
        return 17;
    } else {
        return 11;
    }
}
int CPU::RET()
{
    pc = pop();
    return 10;
}
int CPU::RZ()
{
    if (flags.z) {
        RET();
        return 11;
    } else {
        return 5;
    }
}
int CPU::RNZ()
{
    if (!flags.z) {
        RET();
        return 11;
    } else {
        return 5;
    }
}
int CPU::RC()
{
    if (flags.cy) {
        RET();
        return 11;
    } else {
        return 5;
    }
}
int CPU::RNC()
{
    if (!flags.cy) {
        RET();
        return 11;
    } else {
        return 5;
    }
}
int CPU::PCHL()
{
    pc = hl;
    return 5;
}
int CPU::PUSH_B()
{
    push(bc);
    return 11;
}
int CPU::PUSH_D()
{
    push(de);
    return 11;
}
int CPU::PUSH_H()
{
    push(hl);
    return 11;
}
int CPU::PUSH_PSW()
{
    push(af);
    return 11;
}
int CPU::POP_B()
{
    bc = pop();
    return 10;
}
int CPU::POP_D()
{
    de = pop();
    return 10;
}
int CPU::POP_H()
{
    hl = pop();
    return 10;
}
int CPU::POP_PSW()
{
    af = pop();
    return 10;
}
int CPU::XTHL()
{
    uint16_t temp = pop();
    push(hl);
    hl = temp;
    return 18;
}
int CPU::IN()
{
    read_byte_to_z();
    uint8_t port = z;
    if (port == 3) {
        return 10;
    }
    a = ports[port];
    return 10;
}
int CPU::OUT()
{
    read_byte_to_z();
    uint8_t port = z;
    if (port == 2 || port == 4) {
        return 10;
    }
    ports[port] = a;
    return 10;
}
int CPU::EI()
{
    flags.i = 1;
    return 4;
}
int CPU::NOP()
{
    return 4;
}

//

void CPU::read_bytes_to_wz()
{
    z = mem->get(pc);
    w = mem->get(pc + 1);
    pc += 2;
}
void CPU::read_byte_to_z()
{
    z = mem->get(pc);
    pc++;
}
void CPU::handle_flags(uint32_t result, size_t size, int _flags)
{
    if (_flags & f_z) {
        flags.z = (result == 0);
    }
    if (_flags & f_p) {
        flags.p = (result & 1) == 0;
    }
    if (_flags & f_s) {
        flags.s = (result & (1 << (size - 1))) >> (size - 1) == 1;
    }
    if (_flags & f_cy) {
        flags.cy = (result & ((0xfffffff) << size)) != 0;
    }
}
void CPU::push(uint16_t value)
{
    mem->set(sp - 1, value >> 8);
    mem->set(sp - 2, value);
    sp -= 2;
}
uint16_t CPU::pop()
{
    uint16_t r = (mem->get(sp + 1) << 8) | mem->get(sp);
    sp += 2;
    return r;
}
void CPU::generate_irq(uint16_t addr)
{
    push(pc);
    pc      = addr;
    flags.i = 0;
}
int CPU::exec_instruction(uint8_t ir)
{
    switch (ir) {
        case 0x00:
            return NOP();
        case 0x40:
            return MOV(&b, &b);
        case 0x41:
            return MOV(&b, &c);
        case 0x42:
            return MOV(&b, &d);
        case 0x43:
            return MOV(&b, &e);
        case 0x44:
            return MOV(&b, &h);
        case 0x47:
            return MOV(&b, &a);
        case 0x48:
            return MOV(&c, &b);
        case 0x4f:
            return MOV(&c, &a);
        case 0x57:
            return MOV(&d, &a);
        case 0x5f:
            return MOV(&e, &a);
        case 0x61:
            return MOV(&h, &c);
        case 0x64:
            return MOV(&h, &h);
        case 0x65:
            return MOV(&h, &l);
        case 0x67:
            return MOV(&h, &a);
        case 0x68:
            return MOV(&l, &b);
        case 0x69:
            return MOV(&l, &c);
        case 0x6f:
            return MOV(&l, &a);
        case 0x78:
            return MOV(&a, &b);
        case 0x79:
            return MOV(&a, &c);
        case 0x7a:
            return MOV(&a, &d);
        case 0x7b:
            return MOV(&a, &e);
        case 0x7c:
            return MOV(&a, &h);
        case 0x7d:
            return MOV(&a, &l);
        case 0x46:
            return MOV_from_mem(&b);
        case 0x4e:
            return MOV_from_mem(&c);
        case 0x5e:
            return MOV_from_mem(&e);
        case 0x7e:
            return MOV_from_mem(&a);
        case 0x56:
            return MOV_from_mem(&d);
        case 0x66:
            return MOV_from_mem(&h);
        case 0x70:
            return MOV_to_mem(&b);
        case 0x71:
            return MOV_to_mem(&c);
        case 0x72:
            return MOV_to_mem(&d);
        case 0x73:
            return MOV_to_mem(&e);
        case 0x77:
            return MOV_to_mem(&a);
        case 0x01:
            return LXI(&bc);
        case 0x11:
            return LXI(&de);
        case 0x21:
            return LXI(&hl);
        case 0x31:
            return LXI(&sp);
        case 0x06:
            return MVI(&b);
        case 0x16:
            return MVI(&d);
        case 0x0e:
            return MVI(&c);
        case 0x1e:
            return MVI(&e);
        case 0x26:
            return MVI(&h);
        case 0x2e:
            return MVI(&l);
        case 0x3e:
            return MVI(&a);
        case 0x36:
            return MVI_to_mem();
        case 0x0a:
            return LDAX_B();
        case 0x1a:
            return LDAX_D();
        case 0x3a:
            return LDA();
        case 0x32:
            return STA();
        case 0x02:
            return STAX_B();
        case 0x12:
            return STAX_D();
        case 0x2a:
            return LHLD();
        case 0x22:
            return SHLD();
        case 0xeb:
            return XCHG();
        case 0x80:
            return ADD(&b);
        case 0x81:
            return ADD(&c);
        case 0x82:
            return ADD(&d);
        case 0x83:
            return ADD(&e);
        case 0x85:
            return ADD(&l);
        case 0x86:
            return ADD_M();
        case 0x04:
            return INR(&b);
        case 0x0c:
            return INR(&c);
        case 0x14:
            return INR(&d);
        case 0x1c:
            return INR(&e);
        case 0x24:
            return INR(&h);
        case 0x2c:
            return INR(&l);
        case 0x3c:
            return INR(&a);
        case 0x34:
            return INR_M();
        case 0x05:
            return DCR(&b);
        case 0x0d:
            return DCR(&c);
        case 0x15:
            return DCR(&d);
        case 0x25:
            return DCR(&h);
        case 0x3d:
            return DCR(&a);
        case 0x35:
            return DCR_M();
        case 0x03:
            return INX(&bc);
        case 0x13:
            return INX(&de);
        case 0x23:
            return INX(&hl);
        case 0x1b:
            return DCX(&de);
        case 0x2b:
            return DCX(&hl);
        case 0x09:
            return DAD(&bc);
        case 0x19:
            return DAD(&de);
        case 0x29:
            return DAD(&hl);
        case 0xc6:
            return ADI();
        case 0x8a:
            return ADC(&d);
        case 0x97:
            return SUB(&a);
        case 0xd6:
            return SUI();
        case 0xde:
            return SBI();
        case 0x27:
            return DAA();
        case 0xa0:
            return ANA(&b);
        case 0xa1:
            return ANA(&c);
        case 0xa7:
            return ANA(&a);
        case 0xa6:
            return ANA_M();
        case 0xe6:
            return ANI();
        case 0xa8:
            return XRA(&b);
        case 0xaf:
            return XRA(&a);
        case 0xb0:
            return ORA(&b);
        case 0xb4:
            return ORA(&h);
        case 0xb6:
            return ORA_M();
        case 0xf6:
            return ORI();
        case 0xb8:
            return CMP(&b);
        case 0xbc:
            return CMP(&h);
        case 0xbe:
            return CMP_M();
        case 0xfe:
            return CPI();
        case 0x07:
            return RLC();
        case 0x0f:
            return RRC();
        case 0x1f:
            return RAR();
        case 0x2f:
            return CMA();
        case 0x37:
            return STC();
        case 0xc3:
            read_bytes_to_wz();
            return JMP();
        case 0xca:
            read_bytes_to_wz();
            return JZ();
        case 0xc2:
            read_bytes_to_wz();
            return JNZ();
        case 0xda:
            read_bytes_to_wz();
            return JC();
        case 0xd2:
            read_bytes_to_wz();
            return JNC();
        case 0xfa:
            read_bytes_to_wz();
            return JM();
        case 0xcd:
            read_bytes_to_wz();
            return CALL();
        case 0xcc:
            read_bytes_to_wz();
            return CZ();
        case 0xc4:
            read_bytes_to_wz();
            return CNZ();
        case 0xd4:
            read_bytes_to_wz();
            return CNC();
        case 0xc9:
            return RET();
        case 0xc8:
            return RZ();
        case 0xc0:
            return RNZ();
        case 0xd8:
            return RC();
        case 0xd0:
            return RNC();
        case 0xe9:
            return PCHL();
        case 0xc5:
            return PUSH_B();
        case 0xd5:
            return PUSH_D();
        case 0xe5:
            return PUSH_H();
        case 0xf5:
            return PUSH_PSW();
        case 0xc1:
            return POP_B();
        case 0xd1:
            return POP_D();
        case 0xe1:
            return POP_H();
        case 0xf1:
            return POP_PSW();
        case 0xe3:
            return XTHL();
        case 0xdb:
            return IN();
        case 0xd3:
            return OUT();
        case 0xfb:
            return EI();
        default:
            return 0;
    }
}