from m5.params import *
from m5.SimObject import SimObject

class MyObject(SimObject):
    type = 'MyObject'
    cxx_header = 'learning_gem5/part2/my_object.hh'
    cxx_class = 'gem5::MyObject'

    time_to_wait = Param.Latency("Time before firing the event")
    number_of_fires = Param.Int(1, "Number of times to fire")
    my_exit_object = Param.MyExitObject('my_exit_obj in my_object')

class MyExitObject(SimObject):
    type = 'MyExitObject'
    cxx_header = 'learning_gem5/part2/my_exit_object.hh'
    cxx_class = 'gem5::MyExitObject'

    buffer_size = Param.MemorySize('1kB',
            'Size of buffer to fill')
    write_bandwidth = Param.MemoryBandwidth('100MB/s',
            'Bandwidth to fill the buffer')
