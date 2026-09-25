#pragma once

#include <QHostAddress>
#include <QString>
#include <QStringList>
#include <vector>
#include <map>
#include <memory>
#include "PacketHandlerFramework/NetworkPacketList.hpp" // Needs forward declaration or include

namespace PacketParser {

class NetworkTcpSession; // Forward declaration
class NetworkServiceMetadata; // Forward declaration

class NetworkHost {
public:
    enum class OperatingSystemID { Windows, Linux, UNIX, FreeBSD, NetBSD, Solaris, MacOS, Cisco, Other, Unknown };

private:
    QHostAddress ipAddress;
    QString macAddress; // Using string for MAC address for simplicity
    QStringList hostNameList;
    std::vector<uint16_t> openTcpPortList;
    
    std::map<uint8_t, int> ttlCount;
    std::map<uint8_t, int> ttlDistanceCount;

    std::map<std::string, std::map<std::string, double>> operatingSystemCounterList;
    
    std::shared_ptr<PacketHandlerFramework::NetworkPacketList> sentPackets;
    std::shared_ptr<PacketHandlerFramework::NetworkPacketList> receivedPackets;

    // We'll use void* for sessions as NetworkTcpSession isn't fully implemented
    std::vector<void*> incomingSessionList;
    std::vector<void*> outgoingSessionList;

    // networkServiceMetadataList
    std::map<uint16_t, void*> networkServiceMetadataList; // Mocking with void* for now

    std::map<QString, QString> universalPlugAndPlayFieldList;

    int ttlDistance = 0;
    OperatingSystemID os = OperatingSystemID::Unknown;

public:
    NetworkHost(const QHostAddress& ipAddress);

    QHostAddress IPAddress() const { return ipAddress; }
    QString MacAddress() const { return macAddress; }
    QString HostName() const { return hostNameList.isEmpty() ? "" : hostNameList.first(); }
    OperatingSystemID OS() const { return os; }
    int TtlDistance() const { return ttlDistance; }
    
    std::shared_ptr<PacketHandlerFramework::NetworkPacketList> SentPackets() const { return sentPackets; }
    std::shared_ptr<PacketHandlerFramework::NetworkPacketList> ReceivedPackets() const { return receivedPackets; }

    bool HasUniversalPlugAndPlayFieldList() const { return true; }
    void InitializeUniversalPlugAndPlayFieldList() { }
    bool ContainsUniversalPlugAndPlayField(const QString& field) const { return universalPlugAndPlayFieldList.find(field) != universalPlugAndPlayFieldList.end(); }
    void AddUniversalPlugAndPlayField(const QString& key, const QString& value) { universalPlugAndPlayFieldList[key] = value; }
    void AddHttpUserAgentBanner(const QString& banner) {} // Stubbing for now
    void AddProbableOs(const QString& osID, void* fingerprinter, double probability) {} // Stubbing for now

    std::string ToString() const;
};

} // namespace PacketParser
