from m5.params import *
from m5.SimObject import SimObject

class MicroCacheAdapter(SimObject):
    type = "MicroCacheAdapter"
    cxx_header = "arch/generic/micro_cache_adapter.hh"
    cxx_class = "gem5::MicroCacheAdapter"

    cpu_side_port = ResponsePort("CPU side port, receives requests")
