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
        int id;
        MyCache *owner;
        bool needRetry;
        PacketPtr blockedPacket;

    public:
        CPUSidePort(const std::string &name, int id, MyCache *owner) :
            ResponsePort(name, owner),
            id(id),
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
            owner(owner),
            blockedPacket(nullptr)
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
    void sendRangeChange() const;
    bool handleRequest(PacketPtr pkt, int port_id);
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
    std::unordered_map<Addr, uint8_t*> cacheStore;
    bool accessFunctional(PacketPtr pkt);
    void insert(PacketPtr pkt);

    Tick missTime;
    class MyCacheStats : public statistics::Group
    {
    public:
        MyCacheStats(statistics::Group *parent);
        statistics::Scalar hits;
        statistics::Scalar misses;
        statistics::Histogram missLatency;
        statistics::Formula hitRatio;
    } stats;

public:
    MyCache(const MyCacheParams &params);
    Port &getPort(const std::string &if_name,
            PortID idx = InvalidPortID) override;
};

}
#endif
