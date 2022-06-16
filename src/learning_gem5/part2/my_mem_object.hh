#ifndef __MY_MEM_OBJECT_HH__
#define __MY_MEM_OBJECT_HH__

#include "mem/port.hh"
#include "params/MyMemObject.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class MyMemObject : public SimObject
{
private:
    class CPUSidePort : public ResponsePort
    {
    private:
        MyMemObject *owner;
        bool needRetry;
        PacketPtr blockedPacket;

    public:
        CPUSidePort(const std::string &name, MyMemObject *owner) :
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
        MyMemObject *owner;
        PacketPtr blockedPacket;

    public:
        MemSidePort(const std::string &name, MyMemObject *owner) :
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

    CPUSidePort instPort;
    CPUSidePort dataPort;
    MemSidePort memPort;
    bool blocked;

public:
    MyMemObject(const MyMemObjectParams &params);
    Port &getPort(const std::string &if_name,
            PortID idx = InvalidPortID) override;
};


}

#endif
