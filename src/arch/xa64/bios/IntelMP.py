# Copyright (c) 2008 The Hewlett-Packard Development Company
# All rights reserved.
#
# The license below extends only to copyright in the software and shall
# not be construed as granting a license to any other intellectual
# property including but not limited to intellectual property relating
# to a hardware implementation of the functionality of the software
# licensed hereunder.  You may use the software subject to the license
# terms below provided that you ensure that this notice is replicated
# unmodified and in its entirety in all distributions of the software,
# modified or unmodified, in source code or in binary form.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

from m5.params import *
from m5.SimObject import SimObject

class XA64IntelMPFloatingPointer(SimObject):
    type = 'XA64IntelMPFloatingPointer'
    cxx_class = 'gem5::XA64ISA::intelmp::FloatingPointer'
    cxx_header = 'arch/xa64/bios/intelmp.hh'

    # The minor revision of the spec to support. The major version is assumed
    # to be 1 in accordance with the spec.
    spec_rev = Param.UInt8(4, 'minor revision of the MP spec supported')
    # If no default configuration is used, set this to 0.
    default_config = Param.UInt8(0, 'which default configuration to use')
    imcr_present = Param.Bool(True,
            'whether the IMCR register is present in the APIC')

class XA64IntelMPConfigTable(SimObject):
    type = 'XA64IntelMPConfigTable'
    cxx_class = 'gem5::XA64ISA::intelmp::ConfigTable'
    cxx_header = 'arch/xa64/bios/intelmp.hh'

    spec_rev = Param.UInt8(4, 'minor revision of the MP spec supported')
    oem_id = Param.String("", 'system manufacturer')
    product_id = Param.String("", 'product family')
    oem_table_addr = Param.UInt32(0,
            'pointer to the optional oem configuration table')
    oem_table_size = Param.UInt16(0, 'size of the oem configuration table')
    local_apic = Param.UInt32(0xFEE00000, 'address of the local APIC')

    base_entries = VectorParam.XA64IntelMPBaseConfigEntry([],
            'base configuration table entries')

    ext_entries = VectorParam.XA64IntelMPExtConfigEntry([],
            'extended configuration table entries')

    def add_entry(self, entry):
        if isinstance(entry, XA64IntelMPBaseConfigEntry):
            self.base_entries.append(entry)
        elif isinstance(entry, XA64IntelMPExtConfigEntry):
            self.ext_entries.append(entry)
        else:
            panic("Don't know what type of Intel MP entry %s is." \
                    % entry.__class__.__name__)

class XA64IntelMPBaseConfigEntry(SimObject):
    type = 'XA64IntelMPBaseConfigEntry'
    cxx_class = 'gem5::XA64ISA::intelmp::BaseConfigEntry'
    cxx_header = 'arch/xa64/bios/intelmp.hh'
    abstract = True

class XA64IntelMPExtConfigEntry(SimObject):
    type = 'XA64IntelMPExtConfigEntry'
    cxx_class = 'gem5::XA64ISA::intelmp::ExtConfigEntry'
    cxx_header = 'arch/xa64/bios/intelmp.hh'
    abstract = True

class XA64IntelMPProcessor(XA64IntelMPBaseConfigEntry):
    type = 'XA64IntelMPProcessor'
    cxx_class = 'gem5::XA64ISA::intelmp::Processor'
    cxx_header = 'arch/xa64/bios/intelmp.hh'

    local_apic_id = Param.UInt8(0, 'local APIC id')
    local_apic_version = Param.UInt8(0,
            'bits 0-7 of the local APIC version register')
    enable = Param.Bool(True, 'if this processor is usable')
    bootstrap = Param.Bool(False, 'if this is the bootstrap processor')

    stepping = Param.UInt8(0, 'Processor stepping')
    model = Param.UInt8(0, 'Processor model')
    family = Param.UInt8(0, 'Processor family')

    feature_flags = Param.UInt32(0, 'flags returned by the CPUID instruction')

class XA64IntelMPBus(XA64IntelMPBaseConfigEntry):
    type = 'XA64IntelMPBus'
    cxx_class = 'gem5::XA64ISA::intelmp::Bus'
    cxx_header = 'arch/xa64/bios/intelmp.hh'

    bus_id = Param.UInt8(0, 'bus id assigned by the bios')
    bus_type = Param.String("", 'string that identify the bus type')
    # Legal values for bus_type are [space padded to 6 bytes]:
    #
    # "CBUS", "CBUSII", "EISA", "FUTURE", "INTERN", "ISA", "MBI", "MBII",
    # "MCA", "MPI", "MPSA", "NUBUS", "PCI", "PCMCIA", "TC", "VL", "VME",
    # "XPRESS"

class XA64IntelMPIOAPIC(XA64IntelMPBaseConfigEntry):
    type = 'XA64IntelMPIOAPIC'
    cxx_class = 'gem5::XA64ISA::intelmp::IOAPIC'
    cxx_header = 'arch/xa64/bios/intelmp.hh'

    id = Param.UInt8(0, 'id of this APIC')
    version = Param.UInt8(0, 'bits 0-7 of the version register')

    enable = Param.Bool(True, 'if this APIC is usable')

    address = Param.UInt32(0xfec00000, 'address of this APIC')

class XA64IntelMPInterruptType(Enum):
    map = {'INT' : 0,
           'NMI' : 1,
           'SMI' : 2,
           'ExtInt' : 3
    }

class XA64IntelMPPolarity(Enum):
    map = {'ConformPolarity' : 0,
           'ActiveHigh' : 1,
           'ActiveLow' : 3
    }

class XA64IntelMPTriggerMode(Enum):
    map = {'ConformTrigger' : 0,
           'EdgeTrigger' : 1,
           'LevelTrigger' : 3
    }

class XA64IntelMPIOIntAssignment(XA64IntelMPBaseConfigEntry):
    type = 'XA64IntelMPIOIntAssignment'
    cxx_class = 'gem5::XA64ISA::intelmp::IOIntAssignment'
    cxx_header = 'arch/xa64/bios/intelmp.hh'

    interrupt_type = Param.XA64IntelMPInterruptType('INT', 'type of interrupt')

    polarity = Param.XA64IntelMPPolarity('ConformPolarity', 'polarity')
    trigger = Param.XA64IntelMPTriggerMode('ConformTrigger', 'trigger mode')

    source_bus_id = Param.UInt8(0,
            'id of the bus from which the interrupt signal comes')
    source_bus_irq = Param.UInt8(0,
            'which interrupt signal from the source bus')

    dest_io_apic_id = Param.UInt8(0,
            'id of the IO APIC the interrupt is going to')
    dest_io_apic_intin = Param.UInt8(0,
            'the INTIN pin on the IO APIC the interrupt is connected to')

class XA64IntelMPLocalIntAssignment(XA64IntelMPBaseConfigEntry):
    type = 'XA64IntelMPLocalIntAssignment'
    cxx_class = 'gem5::XA64ISA::intelmp::LocalIntAssignment'
    cxx_header = 'arch/xa64/bios/intelmp.hh'

    interrupt_type = Param.XA64IntelMPInterruptType('INT', 'type of interrupt')

    polarity = Param.XA64IntelMPPolarity('ConformPolarity', 'polarity')
    trigger = Param.XA64IntelMPTriggerMode('ConformTrigger', 'trigger mode')

    source_bus_id = Param.UInt8(0,
            'id of the bus from which the interrupt signal comes')
    source_bus_irq = Param.UInt8(0,
            'which interrupt signal from the source bus')

    dest_local_apic_id = Param.UInt8(0,
            'id of the local APIC the interrupt is going to')
    dest_local_apic_intin = Param.UInt8(0,
            'the INTIN pin on the local APIC the interrupt is connected to')

class XA64IntelMPAddressType(Enum):
    map = {"IOAddress" : 0,
           "MemoryAddress" : 1,
           "PrefetchAddress" : 2
    }

class XA64IntelMPAddrSpaceMapping(XA64IntelMPExtConfigEntry):
    type = 'XA64IntelMPAddrSpaceMapping'
    cxx_class = 'gem5::XA64ISA::intelmp::AddrSpaceMapping'
    cxx_header = 'arch/xa64/bios/intelmp.hh'

    bus_id = Param.UInt8(0, 'id of the bus the address space is mapped to')
    address_type = Param.XA64IntelMPAddressType('IOAddress',
            'address type used to access bus')
    address = Param.Addr(0, 'starting address of the mapping')
    length = Param.UInt64(0, 'length of mapping in bytes')

class XA64IntelMPBusHierarchy(XA64IntelMPExtConfigEntry):
    type = 'XA64IntelMPBusHierarchy'
    cxx_class = 'gem5::XA64ISA::intelmp::BusHierarchy'
    cxx_header = 'arch/xa64/bios/intelmp.hh'

    bus_id = Param.UInt8(0, 'id of the bus being described')
    subtractive_decode = Param.Bool(False,
            'whether this bus contains all addresses not used by its children')
    parent_bus = Param.UInt8(0, 'bus id of this busses parent')

class XA64IntelMPRangeList(Enum):
    map = {"ISACompatible" : 0,
           "VGACompatible" : 1
    }

class XA64IntelMPCompatAddrSpaceMod(XA64IntelMPExtConfigEntry):
    type = 'XA64IntelMPCompatAddrSpaceMod'
    cxx_class = 'gem5::XA64ISA::intelmp::CompatAddrSpaceMod'
    cxx_header = 'arch/xa64/bios/intelmp.hh'

    bus_id = Param.UInt8(0, 'id of the bus being described')
    add = Param.Bool(False,
            'if the range should be added to the original mapping')
    range_list = Param.XA64IntelMPRangeList('ISACompatible',
            'which predefined range of addresses to use')
