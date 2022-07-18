// micro decoder encoder
//
/// TODO: This file is only for test, remove it later
/// Complie:
///     remove #include "arch/generic/decoder.hh" in udencoder.hh
///     class Decoder in udencoder.hh
///     g++ -I./src/ -I./build/X86/ src/arch/x86/udencoder.cc -o udencoder

#include "arch/x86/udencoder.hh"

// TODO: remove
#include <iostream>

using namespace gem5::XA64ISA;

void decode_tmp(MachInst xa64mi) {
    ExtMachInst xa64emi(xa64mi);
    if (xa64emi.opc4 >= opc4_begin) {        // OPC4
        std::cout << "opc4";
    } else if (xa64emi.opc3 >= opc3_begin) { // OPC3
        std::cout << "opc3";
    } else if (xa64emi.opc2 >= opc2_begin) { // OPC2
        std::cout << "opc2";
    } else if (xa64emi.opc1 >= opc1_begin) { // OPC1
        std::cout << "opc1";
    } else {                                 // OPC0
        std::cout << "opc0";
    }
    std::cout << " 0x" << std::hex << (MachInst)xa64mi << std::endl;
}

int main(void) {
    MachInst xa64mi;
    while (1) {
        std::cin >> xa64mi;
        decode_tmp(xa64mi);
    }
    return 0;
}
