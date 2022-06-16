#include "learning_gem5/part2/my_object.hh"

#include "base/trace.hh"
#include "debug/MyDbg.hh"

namespace gem5 {

MyObject::MyObject(const MyObjectParams &params) :
    SimObject(params),
    event([this]{processEvent();}, name()),
    my_exit(params.my_exit_object),
    myName(params.name),
    latency(params.time_to_wait),
    timesLeft(params.number_of_fires)
{
    DPRINTF(MyDbg, "From a MyObject: MyDbg\n");
    panic_if(!my_exit, "must have a non-null MyExitObject");
}

void MyObject::processEvent(void)
{
    timesLeft--;
    DPRINTF(MyDbg, "MyObject: processing the event\n");

    if (timesLeft <= 0) {
        DPRINTF(MyDbg, "Done firing\n");
        my_exit->myExit(myName);
    } else {
        schedule(event, curTick() + latency);
    }
}

void MyObject::startup()
{
    schedule(event, latency);
}

}
