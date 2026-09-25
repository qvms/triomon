#include "WinPCapSniffer.hpp"

namespace NetworkWrapper {

WinPCapSniffer::PacketReceivedHandler WinPCapSniffer::PacketReceived = nullptr;

WinPCapSniffer::WinPCapSniffer(const WinPCapAdapter& adapter, QObject* parent) 
    : QObject(parent), nPacketsReceived(0) 
{
    wpcap = std::make_shared<WinPCapWrapper>();
    if (wpcap->IsOpen()) {
        wpcap->Close();
    }

    if (!wpcap->Open(adapter.NPFName(), 65536, WinPCapNative::PCAP_OPENFLAG_PROMISCUOUS, 0)) {
        throw std::runtime_error(wpcap->LastError().toStdString());
    }

    wpcap->SetMinToCopy(100);

    wpcap->PacketArrival = [this](WinPCapWrapper* sender, PcapHeader* p, const QByteArray& s) {
        this->ReceivePacketListener(sender, p, s);
    };

    int datalink = wpcap->DataLink();
    if (datalink == static_cast<int>(DataLinkType::WTAP_ENCAP_IEEE_802_11)) {
        basePacketType = PacketReceivedEventArgs::PacketTypes::IEEE_802_11Packet;
    } else if (datalink == static_cast<int>(DataLinkType::WTAP_ENCAP_ETHERNET)) {
        basePacketType = PacketReceivedEventArgs::PacketTypes::Ethernet2Packet;
    } else if (datalink == static_cast<int>(DataLinkType::WTAP_ENCAP_IEEE_802_11_WLAN_RADIOTAP)) {
        basePacketType = PacketReceivedEventArgs::PacketTypes::IEEE_802_11RadiotapPacket;
    } else if (datalink == static_cast<int>(DataLinkType::WTAP_ENCAP_RAW_IP) || 
               datalink == static_cast<int>(DataLinkType::WTAP_ENCAP_RAW_IP_2) || 
               datalink == static_cast<int>(DataLinkType::WTAP_ENCAP_RAW_IP_3)) {
        basePacketType = PacketReceivedEventArgs::PacketTypes::IPv4Packet;
    } else if (adapter.ToString().toLower().contains("airpcap")) {
        basePacketType = PacketReceivedEventArgs::PacketTypes::IEEE_802_11Packet;
    } else {
        basePacketType = PacketReceivedEventArgs::PacketTypes::Ethernet2Packet;
    }
}

WinPCapSniffer::~WinPCapSniffer() {
    wpcap->Close();
}

void WinPCapSniffer::StartSniffing() {
    wpcap->StartListen();
}

void WinPCapSniffer::StopSniffing() {
    wpcap->StopListen();
}

void WinPCapSniffer::ReceivePacketListener(WinPCapWrapper* sender, PcapHeader* ph, const QByteArray& data) {
    nPacketsReceived++;
    if (PacketReceived) {
        PacketReceivedEventArgs eventArgs(data, ph->TimeStamp(), basePacketType);
        PacketReceived(this, eventArgs);
    }
}

} // namespace NetworkWrapper
