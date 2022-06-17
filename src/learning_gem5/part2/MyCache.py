from m5.params import *
from m5.proxy import *
from MemObject import MemObject

class MyCache(MemObject):
    type = 'MyCache'
    cxx_header = 'learning_gem5/part2/my_cache.hh'
    cxx_class = 'gem5::MyCache'

    cpu_side = VectorResponsePort('CPU side port, receives requests')
    mem_side = RequestPort('Memory side port, sends requests')

    latency = Cycles(1, "Cycles taken on a hit or to resolve a miss")

    size = MemorySize('16kB', 'The size of the cache')
    system = System(Parent.any, 'The system this cache is part of')
