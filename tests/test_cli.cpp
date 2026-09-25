#include <iostream>
#include <pcap/pcap.h>
#include <QDateTime>
#include "PacketParser/Frame.hpp"
#include "PacketParser/Packets/IPv4Packet.hpp"

int main(int argc, char* argv[]) {
    if (argc < 2) return 1;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* pcap = pcap_open_offline(argv[1], errbuf);
    int dlt = pcap_datalink(pcap);
    PacketParser::DataLinkTypeEnum linkType = PacketParser::DataLinkTypeEnum::WTAP_ENCAP_ETHERNET;
    struct pcap_pkthdr* header;
    const u_char* data;
    while(pcap_next_ex(pcap, &header, &data) >= 0) {
        QDateTime ts; ts.setSecsSinceEpoch(header->ts.tv_sec);
        auto frame = std::make_unique<PacketParser::Frame>(
            ts, data, header->caplen, 1, linkType, true, false
        );
        auto packets = frame->GetPackets();
        for (auto const& [index, p] : packets) {
            if (auto* ipv4 = dynamic_cast<PacketParser::Packets::IPv4Packet*>(p)) {
                std::cout << "IPv4 Source: " << ipv4->SourceIPAddress().toStdString() << std::endl;
                std::cout << "IPv4 Dest: " << ipv4->DestinationIPAddress().toStdString() << std::endl;
            }
        }
    }
    return 0;
}
