from m5.params import *
from m5.proxy import *
from m5.SimObject import SimObject

class MyMemObject(SimObject):
    type = 'MyMemObject'
    cxx_header = 'learning_gem5/part2/my_mem_object.hh'
    cxx_class = 'gem5::MyMemObject'

    # Param.SlavePort
    inst_port = ResponsePort('CPU side port, receives requests')
    # Param.SlavePort
    data_port = ResponsePort('CPU side port, receives requests')
    # Param.MasterPort
    mem_side = RequestPort('Memory side port, sends requests')
