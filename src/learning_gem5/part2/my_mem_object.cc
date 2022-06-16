#include "learning_gem5/part2/my_mem_object.hh"

#include "base/trace.hh"
#include "debug/MyDbg.hh"

namespace gem5 {

MyMemObject::MyMemObject(const MyMemObjectParams &params) :
    SimObject(params),
    instPort(params.name + ".inst_port", this),
    dataPort(params.name + ".data_port", this),
    memPort(params.name + ".mem_side", this),
    blocked(false)
{

}

Port &
MyMemObject::getPort(const std::string &if_name, PortID idx)
{
    panic_if(idx != InvalidPortID,
            "This object doesn't support vector ports");

    // name from MyMemObject.py
    if (if_name == "mem_side") {
        return memPort;
    } else if (if_name == "inst_port") {
        return instPort;
    } else if (if_name == "data_port") {
        return dataPort;
    } else {
        return SimObject::getPort(if_name, idx);
    }
}

void
MyMemObject::CPUSidePort::recvFunctional(PacketPtr pkt) {
    return owner->handleFunctional(pkt);
}

AddrRangeList
MyMemObject::CPUSidePort::getAddrRanges() const {
    return owner->getAddrRanges();
}

void
MyMemObject::handleFunctional(PacketPtr pkt) {
    memPort.sendFunctional(pkt);
}

AddrRangeList
MyMemObject::getAddrRanges() const {
    DPRINTF(MyDbg, "Sending new ranges\n");
    return memPort.getAddrRanges();
}

void
MyMemObject::MemSidePort::recvRangeChange() {
    owner->sendRangeChange();
}

void
MyMemObject::sendRangeChange() {
    instPort.sendRangeChange();
    dataPort.sendRangeChange();
}

bool
MyMemObject::CPUSidePort::recvTimingReq(PacketPtr pkt) {
    if (!owner->handleRequest(pkt)) {
        needRetry = true;
        return false;
    } else {
        return true;
    }
}

bool
MyMemObject::handleRequest(PacketPtr pkt) {
    if (blocked)
        return false;
    DPRINTF(MyDbg, "Got request for addr %#x\n", pkt->getAddr());
    blocked = true;
    memPort.sendPacket(pkt);
    return true;
}

void
MyMemObject::MemSidePort::sendPacket(PacketPtr pkt) {
    panic_if(blockedPacket != nullptr,
            "sendPacked but blockedPacket is not nullptr");
    if (!sendTimingReq(pkt)) {
        blockedPacket = pkt;
    }
}

void
MyMemObject::MemSidePort::recvReqRetry() {
    assert(blockedPacket != nullptr);

    PacketPtr pkt = blockedPacket;
    blockedPacket = nullptr;

    sendPacket(pkt);
}

bool
MyMemObject::MemSidePort::recvTimingResp(PacketPtr pkt) {
    return owner->handleResponse(pkt);
}

bool
MyMemObject::handleResponse(PacketPtr pkt) {
    assert(blocked);
    DPRINTF(MyDbg,
            "Got response for addr %#x\n", pkt->getAddr());

    blocked = false;

    if (pkt->req->isInstFetch()) {
        instPort.sendPacket(pkt);
    } else {
        dataPort.sendPacket(pkt);
    }

    instPort.trySendRetry();
    dataPort.trySendRetry();

    return true;
}

void
MyMemObject::CPUSidePort::sendPacket(PacketPtr pkt) {
    panic_if(blockedPacket != nullptr,
            "sendPacket but blockedPacket is not nullptr");

    if (!sendTimingResp(pkt)) {
        blockedPacket = pkt;
    }
}

void
MyMemObject::CPUSidePort::recvRespRetry() {
    assert(blockedPacket != nullptr);

    PacketPtr pkt = blockedPacket;
    blockedPacket = nullptr;

    sendPacket(pkt);
}

void
MyMemObject::CPUSidePort::trySendRetry() {
    if (needRetry && blockedPacket == nullptr) {
        needRetry = false;
        DPRINTF(MyDbg, "Sending retry req for %d\n", id);
        sendRetryReq();
    }
}

/* MyMemObject* */
/* MyMemObjectParams::create() const { */
/*     return new MyMemObject(this); */
/* } */

}
