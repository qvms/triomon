#include "MicroOlapSniffer.hpp"

#include <cstring>
#include <QDateTime>

namespace NetworkWrapper {

MicroOlapSniffer::MicroOlapSniffer(MicroOlapAdapter* adapterWrapper, QObject* parent)
    : QObject(parent), adapterWrapper(adapterWrapper) {
    
    this->adapter.ThreadCount = this->adapter.MaxThreadCount;
    this->adapter.ConfigHandle = adapterWrapper->getAdapterConfig();
    
    this->packetQueue.ItemsCount = 1000;
    
    // In a real implementation we would route the C callback to handlePacketReceive.
    // For mock purposes:
    // this->packetQueue.OnPacketReceive = new microOLAP::PSSDK::OnPacketReceiveEventHandler(&MicroOlapSniffer::packetQueue_OnPacketReceive);
    
    this->adapter.ReceiveQueue = &this->packetQueue;
}

void MicroOlapSniffer::packetQueue_OnPacketReceive(void* sender, void* ThParam, void* hPacket, void* pPacketData, uint IncPacketSize) {
    if (sender) {
        static_cast<MicroOlapSniffer*>(sender)->handlePacketReceive(hPacket, pPacketData, IncPacketSize);
    }
}

void MicroOlapSniffer::handlePacketReceive(void* hPacket, void* pPacketData, uint IncPacketSize) {
    PacketReceivedEventArgs::PacketTypes packetBaseType;
    
    microOLAP::PSSDK::HNPacket packet;
    packet.Handle = hPacket;
    // Mock initializing these fields properly from pPacketData / hPacket etc in a real scenario
    packet.MediumType = microOLAP::PSSDK::HNNetAdapterType::atEthernet; 
    packet.PacketSize = IncPacketSize;
    packet.PacketData = pPacketData;
    // Mock timestamp
    packet.TimeStamp = 0; 

    if (packet.MediumType == microOLAP::PSSDK::HNNetAdapterType::atEthernet) {
        packetBaseType = PacketReceivedEventArgs::PacketTypes::Ethernet2Packet;
    } else {
        packetBaseType = PacketReceivedEventArgs::PacketTypes::Ethernet2Packet;
    }
    
    QByteArray byteArray(reinterpret_cast<const char*>(packet.PacketData), packet.PacketSize);
    
    QDateTime packetTimestamp;
    // 100-nanosecond intervals since Jan 1, 1601 (Windows file time)
    const qint64 ticksPerMillisecond = 10000;
    const qint64 unixEpochTicks = 621355968000000000LL; // Ticks between 1601 and 1970

    if (packet.TimeStamp < unixEpochTicks) {
        // Assume it's a file time
        qint64 ms = packet.TimeStamp / ticksPerMillisecond;
        packetTimestamp = QDateTime::fromMSecsSinceEpoch(ms).toLocalTime();
    } else {
        // Assume it's .NET ticks since 0001-01-01
        qint64 ms = (packet.TimeStamp - unixEpochTicks) / ticksPerMillisecond;
        packetTimestamp = QDateTime::fromMSecsSinceEpoch(ms).toLocalTime();
    }
    
    PacketReceivedEventArgs eventArgs(byteArray, packetTimestamp, packetBaseType);
    emit PacketReceived(eventArgs);
}

PacketReceivedEventArgs::PacketTypes MicroOlapSniffer::BasePacketType() const {
    return PacketReceivedEventArgs::PacketTypes::Ethernet2Packet;
}

void MicroOlapSniffer::StartSniffing() {
    this->packetQueue.MaxPacketSize = this->adapterWrapper->getAdapterConfig().MaxPacketSize;
    
    microOLAP::PSSDK::PSSDKRES result;
    
    result = this->packetQueue.AllocItems();
    if (result != microOLAP::PSSDK::PSSDKRES::HNERR_OK) {
        throw std::runtime_error("Error during queue memory allocating\nResult = " + std::to_string(static_cast<int>(result)));
    }
    
    result = this->packetQueue.Start();
    if (result != microOLAP::PSSDK::PSSDKRES::HNERR_OK) {
        throw std::runtime_error("Error during start packets queue internal thread\nResult = " + std::to_string(static_cast<int>(result)));
    }
    
    result = this->adapter.OpenAdapter();
    if (result != microOLAP::PSSDK::PSSDKRES::HNERR_OK) {
        this->StopSniffing();
        throw std::runtime_error("Error during open network adapter\nResult = " + std::to_string(static_cast<int>(result)));
    }
    this->adapter.MacFilter = microOLAP::PSSDK::HNMacFilter::mfAll;
}

void MicroOlapSniffer::StopSniffing() {
    if (this->adapter.IsOpened) {
        if (this->adapter.CloseAdapter() != microOLAP::PSSDK::PSSDKRES::HNERR_OK) {
            throw std::runtime_error("Error during close network adapter");
        }
    }
}

} // namespace NetworkWrapper
