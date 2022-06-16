#ifndef __LEARNING_GEM5_MY_OBJECT_HH__
#define __LEARNING_GEM5_MY_OBJECT_HH__

#include "learning_gem5/part2/my_exit_object.hh"
#include "params/MyObject.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class MyObject : public SimObject
{
private:
    void processEvent(void);
    EventFunctionWrapper event;
    MyExitObject* my_exit;
    const std::string myName;
    const Tick latency;
    int timesLeft;

public:
    MyObject(const MyObjectParams &p);

    void startup();
};

}

#endif

