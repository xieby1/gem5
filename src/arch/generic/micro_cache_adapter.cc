#include "arch/generic/micro_cache_adapter.hh"

#include "base/trace.hh"

/* #include "debug/MicroCacheAdapter.hh" */

namespace gem5
{

MicroCacheAdapter::MicroCacheAdapter(const MicroCacheAdapterParams &params) :
    SimObject(params),
    cpuSidePort(params.name + ".cpu_side_port", this),
    blocked(false)
{ }

Port &
MicroCacheAdapter::getPort(const std::string &if_name, PortID idx)
{
    panic_if(idx!=InvalidPortID, "This object doesn't support vector ports");

    if (if_name == "cpu_side_port") {
        return cpuSidePort;
    } else {
        return SimObject::getPort(if_name, idx);
    }
}

void
MicroCacheAdapter::CPUSidePort::sendPacket(PacketPtr pkt)
{}
AddrRangeList
MicroCacheAdapter::CPUSidePort::getAddrRanges() const
{
    return owner->getAddrRanges();
}
void
MicroCacheAdapter::CPUSidePort::trySendRetry()
{}

void
MicroCacheAdapter::CPUSidePort::recvFunctional(PacketPtr pkt)
{
    // TODO: DPRINTF
    std::cout << "0x" << std::hex << pkt->getAddr() << std::endl;
}

bool
MicroCacheAdapter::CPUSidePort::recvTimingReq(PacketPtr pkt)
{return true;}
void
MicroCacheAdapter::CPUSidePort::recvRespRetry()
{};


bool
MicroCacheAdapter::handleRequest(PacketPtr pkt)
{return true;}
void
MicroCacheAdapter::handleFunctional(PacketPtr pkt)
{}
AddrRangeList
MicroCacheAdapter::getAddrRanges() const
{return AddrRangeList();};
void
MicroCacheAdapter::sendRangeChange()
{}
}
