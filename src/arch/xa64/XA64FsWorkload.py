# Copyright (c) 2007-2008 The Hewlett-Packard Development Company
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

from m5.objects.E820 import XA64E820Table, XA64E820Entry
from m5.objects.SMBios import XA64SMBiosSMBiosTable
from m5.objects.IntelMP import XA64IntelMPFloatingPointer, XA64IntelMPConfigTable
from m5.objects.XA64ACPI import XA64ACPIRSDP
from m5.objects.Workload import KernelWorkload

class XA64FsWorkload(KernelWorkload):
    type = 'XA64FsWorkload'
    cxx_header = 'arch/xa64/fs_workload.hh'
    cxx_class = 'gem5::XA64ISA::FsWorkload'

    smbios_table = Param.XA64SMBiosSMBiosTable(
            XA64SMBiosSMBiosTable(), 'table of smbios/dmi information')
    intel_mp_pointer = Param.XA64IntelMPFloatingPointer(
            XA64IntelMPFloatingPointer(),
            'intel mp spec floating pointer structure')
    intel_mp_table = Param.XA64IntelMPConfigTable(
            XA64IntelMPConfigTable(),
            'intel mp spec configuration table')
    acpi_description_table_pointer = Param.XA64ACPIRSDP(
            XA64ACPIRSDP(), 'XA64ACPI root description pointer structure')

class XA64FsLinux(XA64FsWorkload):
    type = 'XA64FsLinux'
    cxx_header = 'arch/xa64/linux/fs_workload.hh'
    cxx_class = 'gem5::XA64ISA::FsLinux'

    e820_table = Param.XA64E820Table(
            XA64E820Table(), 'E820 map of physical memory')
