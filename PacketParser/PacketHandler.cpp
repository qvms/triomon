#include "PacketHandler.hpp"

namespace PacketParser {

PacketHandler::PacketHandler() : networkHostList(std::make_shared<PacketHandlerFramework::NetworkHostList>()), nFramesReceived(0), nBytesReceived(0) {
}

PacketHandler::~PacketHandler() {
}

std::shared_ptr<PacketHandlerFramework::NetworkHostList> PacketHandler::GetNetworkHostList() const {
    return networkHostList; 
}


void PacketHandler::ParseFrame(std::shared_ptr<PacketParser::Frame> frame) {
    nFramesReceived++;
    nBytesReceived += frame->DataLength();

    QString clientIp, serverIp, protocol;
    int clientPort = 0, serverPort = 0;

    for (auto const& [index, p] : frame->GetPackets()) {
        if (auto* ipv4 = dynamic_cast<Packets::IPv4Packet*>(p)) {
            clientIp = ipv4->SourceIPAddress();
            serverIp = ipv4->DestinationIPAddress();
        } else if (auto* ipv6 = dynamic_cast<Packets::IPv6Packet*>(p)) {
            clientIp = ipv6->SourceIPAddress();
            serverIp = ipv6->DestinationIPAddress();
        } else if (auto* tcp = dynamic_cast<Packets::TcpPacket*>(p)) {
            clientPort = tcp->SourcePort();
            serverPort = tcp->DestinationPort();
            protocol = "TCP";
        } else if (auto* udp = dynamic_cast<Packets::UdpPacket*>(p)) {
            clientPort = udp->SourcePort();
            serverPort = udp->DestinationPort();
            protocol = "UDP";
        }
    }

    if (!clientIp.isEmpty() && !serverIp.isEmpty()) {
        auto srcHost = networkHostList->GetNetworkHost(QHostAddress(clientIp));
        if (!srcHost) {
            srcHost = std::make_shared<NetworkHost>(QHostAddress(clientIp));
            networkHostList->Add(srcHost);
        }
        auto dstHost = networkHostList->GetNetworkHost(QHostAddress(serverIp));
        if (!dstHost) {
            dstHost = std::make_shared<NetworkHost>(QHostAddress(serverIp));
            networkHostList->Add(dstHost);
        }

        ParsedSession sess;
        sess.clientIp = clientIp;
        sess.serverIp = serverIp;
        sess.clientPort = clientPort;
        sess.serverPort = serverPort;
        sess.protocol = protocol;
        sess.frameLength = frame->DataLength();
        sess.timestamp = frame->Timestamp();
        sessions.push_back(sess);
    }
}

void PacketHandler::OnAnomalyDetected(const QString& description) {
    // Stub
}

} // namespace PacketParser
