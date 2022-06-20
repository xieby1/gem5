import m5
from m5.objects import *
# from psedoImport import *

system = System()
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()
system.mem_mode = 'timing'
system.mem_ranges = [AddrRange('512MB')]

system.cpu = TimingSimpleCPU()

# system.cache = MyCache()
system.cache = SimpleCache(size='1kB')

system.cpu.icache_port = system.cache.cpu_side
system.cpu.dcache_port = system.cache.cpu_side

system.membus = SystemXBar()

system.cache.mem_side = system.membus.cpu_side_ports

system.cpu.createInterruptController()
system.cpu.interrupts[0].pio = system.membus.mem_side_ports
system.cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
system.cpu.interrupts[0].int_responder = system.membus.mem_side_ports

system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

# what it is this?
system.system_port = system.membus.cpu_side_ports

thispath = os.path.dirname(os.path.realpath(__file__))
binpath = os.path.join(thispath, '../../',
                       'tests/test-progs/hello/bin/x86/linux/hello')

# for gem5 V21 and beyond
system.workload = SEWorkload.init_compatible(binpath)

process = Process()
process.cmd = [binpath]
system.cpu.workload = process
system.cpu.createThreads()

root = Root(full_system = False, system=system)
m5.instantiate()

print('Begining simulatopm!')
exit_event = m5.simulate()
print('Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause()))
