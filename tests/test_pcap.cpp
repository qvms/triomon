#include <QCoreApplication>
#include <QDebug>
#include "PacketParser/Frame.hpp"
#include "PacketParser/Packets/IPv4Packet.hpp"
#include "PacketParser/Packets/TcpPacket.hpp"
#include <pcap.h>

int main(int argc, char* argv[]) {
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* pcap = pcap_open_offline("test.pcap", errbuf);
    if (!pcap) {
        qDebug() << "failed to open test.pcap";
        return 1;
    }
    struct pcap_pkthdr* header;
    const u_char* data;
    int res = pcap_next_ex(pcap, &header, &data);
    if (res >= 0) {
        QDateTime ts; ts.setSecsSinceEpoch(header->ts.tv_sec);
        PacketParser::Frame frame(ts, data, header->caplen, 1, PacketParser::DataLinkTypeEnum::WTAP_ENCAP_ETHERNET, true, false);
        for (auto const& [idx, p] : frame.GetPackets()) {
            qDebug() << "Packet at" << idx << ":" << p->PacketTypeDescription();
        }
    }
    return 0;
}
