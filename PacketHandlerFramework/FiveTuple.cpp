#include "FiveTuple.hpp"
#include <sstream>

namespace PacketHandlerFramework {

FiveTuple::FiveTuple(NetworkHost* clientHost, uint16_t clientPort, NetworkHost* serverHost, uint16_t serverPort, TransportProtocol transport)
    : ClientHost(clientHost), ServerHost(serverHost), ClientPort(clientPort), ServerPort(serverPort), Transport(transport) {
    if (ServerHost && ServerHost->IPAddress) {
        ServerEndPoint = std::make_unique<System::Net::IPEndPoint>(ServerHost->IPAddress, ServerPort);
    }
}

FiveTuple* FiveTuple::ReverseClientServer() const {
    return new FiveTuple(ServerHost, ServerPort, ClientHost, ClientPort, Transport);
}

std::string FiveTuple::ToString() const {
    return ToString(false);
}

std::string FiveTuple::ToString(bool printClientFirst) const {
    if (printClientFirst) {
        if (clientServerString.empty()) {
            std::ostringstream sb;
            if (ClientHost && ClientHost->IPAddress) sb << ClientHost->IPAddress->ToString();
            sb << " " << static_cast<int>(Transport) << " " << ClientPort << " - ";
            if (ServerHost && ServerHost->IPAddress) sb << ServerHost->IPAddress->ToString();
            sb << " " << static_cast<int>(Transport) << " " << ServerPort;
            clientServerString = sb.str();
        }
        return clientServerString;
    } else {
        if (serverClientString.empty()) {
            std::ostringstream sb;
            if (ServerHost && ServerHost->IPAddress) sb << ServerHost->IPAddress->ToString();
            sb << " " << static_cast<int>(Transport) << " " << ServerPort << " - ";
            if (ClientHost && ClientHost->IPAddress) sb << ClientHost->IPAddress->ToString();
            sb << " " << static_cast<int>(Transport) << " " << ClientPort;
            serverClientString = sb.str();
        }
        return serverClientString;
    }
}

int FiveTuple::GetHashCode() const {
    int clientHash = ClientHost ? ClientHost->GetHashCode() : 0;
    int serverHash = ServerHost ? ServerHost->GetHashCode() : 0;
    return (clientHash ^ ClientPort) + (serverHash ^ ServerPort) + (static_cast<uint8_t>(Transport) << 16);
}

bool FiveTuple::EqualsIgnoreDirection(const FiveTuple* other) const {
    if (!other) return false;
    return EqualsIgnoreDirection(other->ClientHost ? other->ClientHost->IPAddress : nullptr, other->ClientPort, 
                                 other->ServerHost ? other->ServerHost->IPAddress : nullptr, other->ServerPort, other->Transport);
}

bool FiveTuple::EqualsIgnoreDirection(const System::Net::IPAddress* ipA, uint16_t portA, const System::Net::IPAddress* ipB, uint16_t portB, TransportProtocol transport) const {
    if (Transport != transport) return false;
    if (!ClientHost || !ClientHost->IPAddress || !ServerHost || !ServerHost->IPAddress) return false;
    
    if (ClientHost->IPAddress->Equals(ipA) && ServerHost->IPAddress->Equals(ipB) && ClientPort == portA && ServerPort == portB)
        return true;
    if (ClientHost->IPAddress->Equals(ipB) && ServerHost->IPAddress->Equals(ipA) && ClientPort == portB && ServerPort == portA)
        return true;
    return false;
}

bool FiveTuple::HasIPEndpoint(const System::Net::IPEndPoint* endPoint, const TransportProtocol* transportProtocol) const {
    if (!endPoint || !ClientHost || !ClientHost->IPAddress) return false;
    if (endPoint->get_AddressFamily() != ClientHost->IPAddress->GetAddressFamily())
        return false;
    return HasIPEndpoint(endPoint->Address, static_cast<uint16_t>(endPoint->Port), transportProtocol);
}

bool FiveTuple::HasIPEndpoint(const System::Net::IPAddress* endPointIP, uint16_t endPointPort, const TransportProtocol* transportProtocol) const {
    if (transportProtocol && *transportProtocol != Transport) return false;
    if (!ClientHost || !ClientHost->IPAddress || !ServerHost || !ServerHost->IPAddress) return false;

    if (ClientHost->IPAddress->Equals(endPointIP) && ClientPort == endPointPort)
        return true;
    if (ServerHost->IPAddress->Equals(endPointIP) && ServerPort == endPointPort)
        return true;
    return false;
}

}
