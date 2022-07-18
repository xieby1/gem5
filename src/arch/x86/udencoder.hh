// micro decoder encoder
#ifndef __ARCH_X86_UDENCODER_HH__
#define __ARCH_X86_UDENCODER_HH__

#include "arch/generic/decoder.hh"
#include "arch/x86/utypes.hh"

namespace gem5
{

namespace XA64ISA
{

enum OPC4
{
    opc4_begin = 0b100,

    opc4_mul2 = opc4_begin,
    opc4_mul2_f,
    opc4_mul2_i,
    opc4_mul2_if,
    opc4_shliadd,

    opc4_end = 0b1111
};

enum OPC3
{
    opc3_begin = 1,

    opc3_add = opc3_begin,
    opc3_add_f,
    opc3_add_i,
    opc3_add_if,
    opc3_adc,
    opc3_adc_f,
    opc3_adc_i,
    opc3_adc_if,
    opc3_ld,
    opc3_ld_i,
    opc3_and,
    opc3_and_f,
    opc3_and_i,
    opc3_and_if,
    opc3_rotl,
    opc3_rotl_f, // TODO: flags
    opc3_rotli,
    opc3_rotli_f,
    opc3_rotr,
    opc3_rotr_f,
    opc3_rotri,
    opc3_rotri_f,
    opc3_shl,
    opc3_shl_f,
    opc3_shli,
    opc3_shli_f,
    opc3_shr,
    opc3_shr_f,
    opc3_shri,
    opc3_shri_f,
    opc3_mul,
    opc3_mul_f,
    opc3_mul_i,
    opc3_mul_if,
    opc3_or,
    opc3_or_f,
    opc3_or_i,
    opc3_or_if,
    opc3_st,
    opc3_sar,
    opc3_sar_f,
    opc3_sar_i,
    opc3_sar_if,
    opc3_sub,
    opc3_sub_f,
    opc3_sub_i,
    opc3_sub_if,
    opc3_sbb,
    opc3_sbb_f,
    opc3_sbb_i,
    opc3_sbb_if,
    opc3_xor,
    opc3_xor_f,
    opc3_xor_i,
    opc3_xor_if,
    opc3_shl2,
    opc3_shl2_f, // TODO: flags
    opc3_shl2_i,
    opc3_shl2_if, // TODO: flags
    opc3_cmpxchg,
    opc3_cmpxchg_f, // TODO: flags

    opc3_end = 0b11111111
};

enum OPC2
{
    opc2_begin = 1,

    opc2_bswap = opc2_begin,
    opc2_clz,
    opc2_clz_f, // TODO: flags
    opc2_ctz,
    opc2_ctz_f, // TODO: flags
    opc2_mov,
    opc2_mov_i,
    opc2_neg,
    opc2_neg_f, // TODO: flags
    opc2_not,
    opc2_not_f, // TODO: flags
    opc2_ld,
    opc2_st,

    opc2_end = 0b111111
};

enum OPC1
{
    opc1_begin = 1,

    opc1_end = 0b111111
};

enum OPC0
{
    opc0_begin = 0,

    opc0_invalid = opc0_begin,
    opc0_nop,

    opc0_end = 0b111111
};

class Decoder : public InstDecoder
{
    StaticInstPtr decodeInst(ExtMachInst mach_inst);
};

} // namespace XA64ISA

} // namespace gem5

#endif // __ARCH_X86_UDENCODER_HH__
