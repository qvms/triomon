#pragma once

#include <QString>
#include <QObject>
#include <QByteArray>
#include <QDateTime>
#include <exception>

#include "NetworkWrapper/ISniffer.hpp"
#include "MicroOlapAdapter.hpp"
#include "SharedUtils/Pcap/PacketReceivedEventArgs.hpp"

// Mock microOLAP SDK to allow compilation
namespace microOLAP {
namespace PSSDK {

    enum class HNNetAdapterType {
        atEthernet
    };

    struct HNPacket {
        void* Handle;
        HNNetAdapterType MediumType;
        int PacketSize;
        void* PacketData;
        qint64 TimeStamp;
    };

    class OnPacketReceiveEventHandler {
    public:
        using Func = void(*)(void*, void*, void*, void*, uint);
        OnPacketReceiveEventHandler(Func f) : func(f) {}
        Func func;
    };

    struct HNQueue {
        int ItemsCount;
        int MaxPacketSize;
        OnPacketReceiveEventHandler* OnPacketReceive;
        
        PSSDKRES AllocItems() { return PSSDKRES(); }
        PSSDKRES Start() { return PSSDKRES(); }
    };

    enum class HNMacFilter {
        mfAll
    };

    class HNAdapter {
    public:
        int ThreadCount;
        int MaxThreadCount;
        HNAdapterConfig ConfigHandle;
        HNQueue* ReceiveQueue;
        bool IsOpened;
        HNMacFilter MacFilter;

        PSSDKRES OpenAdapter() { return PSSDKRES(); }
        PSSDKRES CloseAdapter() { return PSSDKRES(); }
    };

} // namespace PSSDK
} // namespace microOLAP

namespace NetworkWrapper {

class MicroOlapSniffer : public QObject, public ISniffer {
    Q_OBJECT
private:
    MicroOlapAdapter* adapterWrapper;
    microOLAP::PSSDK::HNAdapter adapter;
    microOLAP::PSSDK::HNQueue packetQueue;

public:
    explicit MicroOlapSniffer(MicroOlapAdapter* adapterWrapper, QObject* parent = nullptr);

    PacketReceivedEventArgs::PacketTypes BasePacketType() const;

    void StartSniffing() override;
    void StopSniffing() override;
    
    // In C# it was a static event. We can implement it as a Qt signal.
signals:
    void PacketReceived(const PacketReceivedEventArgs& args);

public:
    // Helper function for the C callback equivalent
    static void packetQueue_OnPacketReceive(void* sender, void* ThParam, void* hPacket, void* pPacketData, uint IncPacketSize);
    
    // We'll keep a pointer to the singleton or instance if needed, but since the event was static in C#
    // we can use a static pointer to emit, or better, route the callback correctly if we have a way.
    // Given the C# code didn't handle multiple instances perfectly for static events from instance callback,
    // we will pass the instance as sender.
    void handlePacketReceive(void* hPacket, void* pPacketData, uint IncPacketSize);
};

} // namespace NetworkWrapper
