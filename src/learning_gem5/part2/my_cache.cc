#include "learning_gem5/part2/my_cache.hh"

#include "base/random.hh"
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
    } else {
        Addr addr = pkt->getAddr();
        Addr block_addr = pkt->getBlockAddr(blockSize);
        unsigned size = pkt->getSize();
        if (addr == block_addr && size == blockSize) {
            DPRINTF(MyDbg, "forwarding packet\n");
            memPort.sendPacket(pkt);
        } else {
            DPRINTF(MyDbg, "Upgrading packet to block size\n");
            panic_if(addr - block_addr + size > blockSize,
                    "Cannot handle accesses that span multiple cache lines");
            assert(pkt->needsResponse());
            MemCmd cmd;
            if (pkt->isWrite() || pkt->isRead()) {
                cmd = MemCmd::ReadReq;
            } else {
                panic("Unknown packet type in upgrade size");
            }
            PacketPtr new_pkt = new Packet(pkt->req, cmd, blockSize);
            new_pkt->allocate();
            assert(new_pkt->getAddr() == new_pkt->getBlockAddr(blockSize));
            originalPacket = pkt;
            DPRINTF(MyDbg, "forwarding packet\n");
            memPort.sendPacket(new_pkt);
        }
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

bool
MyCache::handleResponse(PacketPtr pkt) {
    assert(blocked);
    DPRINTF(MyDbg, "Got response for addr %#x\n", pkt->getAddr());
    insert(pkt);
    if (originalPacket != nullptr) {
        [[maybe_unused]] bool hit = accessFunctional(originalPacket);
        panic_if(!hit, "Should always hit after inserting");
        originalPacket->makeResponse();
        delete  pkt;
        pkt = originalPacket;
        originalPacket = nullptr;
    }
    sendResponse(pkt);
    return true;
}

bool
MyCache::accessFunctional(PacketPtr pkt)
{
    Addr block_addr = pkt->getBlockAddr(blockSize);
    auto it = cacheStore.find(block_addr);
    if (it != cacheStore.end()) {
        if (pkt->isWrite()) {
            pkt->writeDataToBlock(it->second, blockSize);
        } else if (pkt->isRead()) {
            pkt->setDataFromBlock(it->second, blockSize);
        } else {
            panic("Unknow packet type!");
        }
        return true;
    } else {
        return false;
    }
}

void
MyCache::insert(PacketPtr pkt)
{
    assert(pkt->getAddr() == pkt->getBlockAddr(blockSize));
    assert(cacheStore.find(pkt->getAddr()) == cacheStore.end());
    assert(pkt->isResponse());

    if (cacheStore.size() >= capacity) {
        int bucket, bucket_size;
        do {
            bucket = random_mt.random(0, (int)cacheStore.bucket_count() - 1);
        } while ((bucket_size = cacheStore.bucket_size(bucket))==0);
        auto block = std::next(cacheStore.begin(bucket),
                random_mt.random(0, bucket_size - 1));
        DPRINTF(MyDbg, "Removing addr %#x\n", block->first);

        RequestPtr req = std::make_shared<Request>(
                block->first, blockSize, 0, 0);
        PacketPtr new_pkt = new Packet(req, MemCmd::WritebackDirty, blockSize);
        new_pkt->dataDynamic(block->second);
        DPRINTF(MyDbg, "Writing packet back %s\n", pkt->print());
        memPort.sendPacket(new_pkt);
        cacheStore.erase(block->first);
    }
    DPRINTF(MyDbg, "Inserting %s\n", pkt->print());
    DDUMP(MyDbg, pkt->getConstPtr<uint8_t>(), blockSize);
    uint8_t *data = new uint8_t[blockSize];
    cacheStore[pkt->getAddr()] = data;
    pkt->writeDataToBlock(data, blockSize);
}

void
MyCache::MemSidePort::sendPacket(PacketPtr pkt)
{
    // Note: This flow control is very simple since the cache is blocking.

    panic_if(blockedPacket != nullptr, "Should never try to send if blocked!");

    // If we can't send the packet across the port, store it for later.
    if (!sendTimingReq(pkt)) {
        blockedPacket = pkt;
    }
}

bool
MyCache::MemSidePort::recvTimingResp(PacketPtr pkt)
{
    // Just forward to the cache.
    return owner->handleResponse(pkt);
}

void
MyCache::MemSidePort::recvReqRetry()
{
    // We should have a blocked packet if this function is called.
    assert(blockedPacket != nullptr);

    // Grab the blocked packet.
    PacketPtr pkt = blockedPacket;
    blockedPacket = nullptr;

    // Try to resend it. It's possible that it fails again.
    sendPacket(pkt);
}

void
MyCache::MemSidePort::recvRangeChange()
{
    owner->sendRangeChange();
}

void
MyCache::CPUSidePort::sendPacket(PacketPtr pkt)
{
    // Note: This flow control is very simple since the cache is blocking.

    panic_if(blockedPacket != nullptr, "Should never try to send if blocked!");

    // If we can't send the packet across the port, store it for later.
    DPRINTF(MyDbg, "Sending %s to CPU\n", pkt->print());
    if (!sendTimingResp(pkt)) {
        DPRINTF(MyDbg, "failed!\n");
        blockedPacket = pkt;
    }
}

AddrRangeList
MyCache::CPUSidePort::getAddrRanges() const
{
    return owner->getAddrRanges();
}

void
MyCache::CPUSidePort::trySendRetry()
{
    if (needRetry && blockedPacket == nullptr) {
        // Only send a retry if the port is now completely free
        needRetry = false;
        DPRINTF(MyDbg, "Sending retry req.\n");
        sendRetryReq();
    }
}

void
MyCache::CPUSidePort::recvFunctional(PacketPtr pkt)
{
    // Just forward to the cache.
    return owner->handleFunctional(pkt);
}

bool
MyCache::CPUSidePort::recvTimingReq(PacketPtr pkt)
{
    DPRINTF(MyDbg, "Got request %s\n", pkt->print());

    if (blockedPacket || needRetry) {
        // The cache may not be able to send a reply if this is blocked
        DPRINTF(MyDbg, "Request blocked\n");
        needRetry = true;
        return false;
    }
    // Just forward to the cache.
    if (!owner->handleRequest(pkt, id)) {
        DPRINTF(MyDbg, "Request failed\n");
        // stalling
        needRetry = true;
        return false;
    } else {
        DPRINTF(MyDbg, "Request succeeded\n");
        return true;
    }
}

void
MyCache::CPUSidePort::recvRespRetry()
{
    // We should have a blocked packet if this function is called.
    assert(blockedPacket != nullptr);

    // Grab the blocked packet.
    PacketPtr pkt = blockedPacket;
    blockedPacket = nullptr;

    DPRINTF(MyDbg, "Retrying response pkt %s\n", pkt->print());
    // Try to resend it. It's possible that it fails again.
    sendPacket(pkt);

    // We may now be able to accept new packets
    trySendRetry();
}

AddrRangeList
MyCache::getAddrRanges() const
{
    DPRINTF(MyDbg, "Sending new ranges\n");
    // Just use the same ranges as whatever is on the memory side.
    return memPort.getAddrRanges();
}

void
MyCache::sendRangeChange() const
{
    for (auto& port : cpuPorts) {
        port.sendRangeChange();
    }
}

void
MyCache::handleFunctional(PacketPtr pkt)
{
    if (accessFunctional(pkt)) {
        pkt->makeResponse();
    } else {
        memPort.sendFunctional(pkt);
    }
}


}
