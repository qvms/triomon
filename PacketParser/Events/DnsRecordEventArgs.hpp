#pragma once

namespace PacketParser {
    namespace Packets {
        class IPv4Packet;
        class UdpPacket;
        namespace DnsPacket {
            class ResourceRecord;
        }
    }
    class NetworkHost;

namespace Events {

class DnsRecordEventArgs {
public:
    Packets::DnsPacket::ResourceRecord* Record;
    NetworkHost* DnsServer;
    NetworkHost* DnsClient;
    Packets::IPv4Packet* IpPacket;
    Packets::UdpPacket* UdpPacket;

    DnsRecordEventArgs(Packets::DnsPacket::ResourceRecord* record, NetworkHost* dnsServer, NetworkHost* dnsClient, Packets::IPv4Packet* ipPacket, Packets::UdpPacket* udpPacket)
        : Record(record), DnsServer(dnsServer), DnsClient(dnsClient), IpPacket(ipPacket), UdpPacket(udpPacket) {}
};

}
}