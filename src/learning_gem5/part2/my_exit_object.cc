#include "learning_gem5/part2/my_exit_object.hh"

#include "base/trace.hh"
#include "debug/MyDbg.hh"
#include "sim/sim_exit.hh"

namespace gem5 {

MyExitObject::MyExitObject(const MyExitObjectParams &params):
    SimObject(params),
    event(*this),
    bandwidth(params.write_bandwidth),
    bufferSize(params.buffer_size),
    buffer(nullptr),
    bufferUsed(0)
{
    buffer = new char[bufferSize];
    DPRINTF(MyDbg, "Create a MyExitObject, bandwidth %f, bufferSize%d\n",
            bandwidth, bufferSize);
}

MyExitObject::~MyExitObject() {
    delete [] buffer;
}

void MyExitObject::processEvent() {
    DPRINTF(MyDbg, "MyExitObject processes a event\n");
    fillBuffer();
}

void MyExitObject::myExit(std::string msg) {
    message = "exit: " + msg;
    DPRINTF(MyDbg, "%s\n", message);
    fillBuffer();
}

void MyExitObject::fillBuffer()
{
    assert(message.length() > 0);

    int bytes_copied = 0;
    for (auto it = message.begin();
            it < message.end() && bufferUsed < bufferSize - 1;
            it++, bufferUsed++, bytes_copied++) {
        buffer[bufferUsed] = *it;
    }

    if (bufferUsed < bufferSize - 1) {
        DPRINTF(MyDbg, "Scheduling another fillBuffer in %d ticks\n",
                bytes_copied * bandwidth);
        schedule(event, curTick() + bytes_copied * bandwidth);
    } else {
        DPRINTF(MyDbg, "buffer full\n");
        exitSimLoop(buffer, 0, curTick() + bytes_copied * bandwidth);
    }
}

}
