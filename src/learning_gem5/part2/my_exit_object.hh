#ifndef __LEARNING_GEM5_MY_EXIT_OBJECT_HH__
#define __LEARNING_GEM5_MY_EXIT_OBJECT_HH__

#include <string>

#include "params/MyExitObject.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class MyExitObject : public SimObject
{
private:
    void processEvent();
    void fillBuffer();
    EventWrapper<MyExitObject, &MyExitObject::processEvent> event;
    float bandwidth;
    int bufferSize;
    char *buffer;
    std::string message;
    int bufferUsed;

public:
    MyExitObject(const MyExitObjectParams &params);
    ~MyExitObject();
    void myExit(std::string msg);
};

}

#endif
