#ifndef __ARCH_X86_UTYPES_HH__
#define __ARCH_X86_UTYPES_HH__

#include "base/bitunion.hh"

namespace gem5
{

namespace XA64ISA
{

typedef uint32_t MachInst;

BitUnion32(ExtMachInst)
    Bitfield<31, 30> ex;
    Bitfield<29, 28> sz;

    Bitfield<27, 24> opc4;
    Bitfield<25, 18> opc3;
    Bitfield<17, 12> opc2;
    Bitfield<11,  6> opc1;
    Bitfield<5 ,  0> opc0;

    Bitfield<23, 18> opr3;
    Bitfield<17, 12> opr2;
    Bitfield<11,  6> opr1;
    Bitfield<5 ,  0> opr0;
EndBitUnion(ExtMachInst)

} // namespace XA64ISA

} // namespace gem5

#endif // __ARCH_X86_UTYPES_HH__
