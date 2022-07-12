#ifndef __ARCH_GENERIC_MICRO_CACHE_ADAPTER_HH__
#define __ARCH_GENERIC_MICRO_CACHE_ADAPTER_HH__

#include "mem/port.hh"
#include "params/MicroCacheAdapter.hh"
#include "sim/sim_object.hh"

namespace gem5
{

class MicroCacheAdapter : public SimObject
{
  private:
    class CPUSidePort : public ResponsePort
    {
      private:
        MicroCacheAdapter *owner;
        bool needRetry;
        PacketPtr blockedPacket;
      public:
        CPUSidePort(const std::string& name, MicroCacheAdapter *owner) :
            ResponsePort(name, owner), owner(owner), needRetry(false),
            blockedPacket(nullptr)
        { }
        void sendPacket(PacketPtr pkt);
        AddrRangeList getAddrRanges() const override;
        void trySendRetry();
      protected:
        Tick recvAtomic(PacketPtr pkt) override
        { panic("recvAtomic unimpl."); }
        void recvFunctional(PacketPtr pkt) override;
        bool recvTimingReq(PacketPtr pkt) override;
        void recvRespRetry() override;
    };
    bool handleRequest(PacketPtr pkt);
    void handleFunctional(PacketPtr pkt);
    AddrRangeList getAddrRanges() const;
    void sendRangeChange();
    CPUSidePort cpuSidePort;
    bool blocked;
  public:
    MicroCacheAdapter(const MicroCacheAdapterParams &params);
    Port &getPort(const std::string &if_name,
            PortID idx=InvalidPortID) override;
};

} // namespace gem5

#endif // __ARCH_GENERIC_MICRO_CACHE_ADAPTER_HH__
