#include "learning_gem5/part2/my_cache.hh"

#include "base/trace.hh"
#include "debug/MyDbg.hh"
#include "sim/system.hh"

namespace gem5 {

MyCache::MyCache(const MyCacheParams &params) :
    ClockedObject(params),
    latency(params.latency),
    blockSize(params.system->cacheLineSize()),
    capacity(params.size / blockSize),
    memPort(params.name + ".mem_side", this),
    blocked(false),
    originalPacket(nullptr),
    waitingPortId(-1)
    /* stats(this) */
{
    for (int i=0; i<params.port_cpu_side_connection_count; i++) {
        cpuPorts.emplace_back(
            name() + csprintf(".cpu_side[%d]", i), i, this
        );
    }
}

Port &
MyCache::getPort(const std::string &if_name, PortID idx) {
    if (if_name == "mem_side") {
        panic_if(idx != InvalidPortID,
                "Mem side of simple cache not a vector port");
        return memPort;
    } else if (if_name == "cpu_side" && idx < cpuPorts.size()) {
        return cpuPorts[idx];
    } else {
        return ClockedObject::getPort(if_name, idx);
    }
}

bool
MyCache::handleRequest(PacketPtr pkt, int port_id) {
    if (blocked) {
        return false;
    }
    DPRINTF(MyDbg, "Got request for addr %#x\n", pkt->getAddr());
    blocked = true;
    assert(waitingPortId == -1);
    waitingPortId = port_id;
    schedule(new EventFunctionWrapper([this, pkt]{accessTiming(pkt);},
                name() + ".accessEvent", true),
            clockEdge(latency));
    return true;
}

void MyCache::accessTiming(PacketPtr pkt) {
    bool hit = accessFunctional(pkt);

    DPRINTF(MyDbg, "%s for packet: %s\n",
            hit ? "Hit" : "Miss",
            pkt->print());
    if (hit) {
        DDUMP(MyDbg, pkt->getConstPtr<uint8_t>(), pkt->getSize());
        pkt->makeResponse();
        sendResponse(pkt);
    }
}

void MyCache::sendResponse(PacketPtr pkt) {
    assert(blocked);
    DPRINTF(MyDbg, "Sending resp for addr %#x\n", pkt->getAddr());
    int port = waitingPortId;
    blocked = false;
    waitingPortId = -1;
    cpuPorts[port].sendPacket(pkt);
    for (auto &port : cpuPorts) {
        port.trySendRetry();
    }
}

}
