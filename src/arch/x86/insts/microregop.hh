/*
 * Copyright (c) 2007 The Hewlett-Packard Development Company
 * All rights reserved.
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ARCH_X86_INSTS_MICROREGOP_HH__
#define __ARCH_X86_INSTS_MICROREGOP_HH__

#include "arch/x86/insts/microop.hh"
#include "arch/x86/insts/microop_args.hh"

namespace gem5
{

namespace X86ISA
{

class RegOpBase : public X86MicroopBase
{
  protected:
    const uint16_t ext;

    RegOpBase(ExtMachInst mach_inst, const char *mnem, const char *inst_mnem,
            uint64_t set_flags, OpClass op_class, uint8_t data_size,
            uint16_t _ext) :
        X86MicroopBase(mach_inst, mnem, inst_mnem, set_flags, op_class),
        ext(_ext), dataSize(data_size),
        foldOBit((data_size == 1 && !mach_inst.rex.present) ? 1 << 7 : 0)
    {}

    //Figure out what the condition code flags should be.
    uint64_t genFlags(uint64_t old_flags, uint64_t flag_mask,
            uint64_t _dest, uint64_t _src1, uint64_t _src2,
            bool subtract=false)const ;

  public:
    const uint8_t dataSize;
    const RegIndex foldOBit;
};

template <typename ...Operands>
using RegOpT = InstOperands<RegOpBase, Operands...>;

} // namespace X86ISA
} // namespace gem5

#endif //__ARCH_X86_INSTS_MICROREGOP_HH__
