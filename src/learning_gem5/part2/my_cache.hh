#ifndef __MY_CACHE_HH__
#define __MY_CACHE_HH__

#include "mem/port.hh"
#include "params/MyCache.hh"
#include "sim/clocked_object.hh"

namespace gem5 {

class MyCache : public ClockedObject
{
private:
    class CPUSidePort : public ResponsePort
    {
    private:
        MyCache *owner;
        bool needRetry;
        PacketPtr blockedPacket;

    public:
        CPUSidePort(const std::string &name, MyCache *owner) :
            ResponsePort(name, owner),
            owner(owner),
            needRetry(false),
            blockedPacket(nullptr)
        {

        }
        AddrRangeList getAddrRanges() const override;
        void sendPacket(PacketPtr ptr);
        void trySendRetry();

    protected:
        Tick recvAtomic(PacketPtr pkt) override { panic("recvAtomic unimpl.");}
        void recvFunctional(PacketPtr pkt) override;
        bool recvTimingReq(PacketPtr pkt) override;
        void recvRespRetry() override;
    };

    class MemSidePort : public RequestPort
    {
    private:
        MyCache *owner;
        PacketPtr blockedPacket;

    public:
        MemSidePort(const std::string &name, MyCache *owner) :
            RequestPort(name, owner),
            owner(owner)
        {

        }
        void sendPacket(PacketPtr pkt);

    protected:
        bool recvTimingResp(PacketPtr pkt) override;
        void recvReqRetry() override;
        void recvRangeChange() override;
    };

    void handleFunctional(PacketPtr pkt);
    AddrRangeList getAddrRanges() const;
    void sendRangeChange();
    bool handleRequest(PacketPtr pkt);
    bool handleResponse(PacketPtr pkt);
    void sendResponse(PacketPtr pkt);
    void accessTiming(PacketPtr pkt);

    const Cycles latency;
    const unsigned blockSize;
    const unsigned capacity;
    std::vector<CPUSidePort> cpuPorts;
    MemSidePort memPort;
    bool blocked;
    PacketPtr originalPacket;
    int waitingPortId;
public:
    MyCache(const MyCacheParams &params);
    Port &getPort(const std::string &if_name,
            PortID idx = InvalidPortID) override;
}

}
#endif
