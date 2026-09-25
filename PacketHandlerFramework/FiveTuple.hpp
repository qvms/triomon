#ifndef FIVETUPLE_HPP
#define FIVETUPLE_HPP

#include <QHostAddress>
#include <QString>

class FiveTuple {
public:
    enum class TransportProtocol {
        TCP,
        UDP
    };

    QHostAddress sourceIp;
    QHostAddress destinationIp;
    quint16 sourcePort;
    quint16 destinationPort;
    TransportProtocol transport;

    // Added for SessionEventArgs compatibility based on C#
    QString ClientHost;
    QString ServerHost;
    quint16 ClientPort;
    quint16 ServerPort;
    TransportProtocol Transport;

    FiveTuple(const QHostAddress& sourceIp, const QHostAddress& destinationIp, quint16 sourcePort, quint16 destinationPort, TransportProtocol transport)
        : sourceIp(sourceIp), destinationIp(destinationIp), sourcePort(sourcePort), destinationPort(destinationPort), transport(transport),
          ClientHost(sourceIp.toString()), ServerHost(destinationIp.toString()), ClientPort(sourcePort), ServerPort(destinationPort), Transport(transport) {}

    QHostAddress getSourceIp() const { return sourceIp; }
    QHostAddress getDestinationIp() const { return destinationIp; }
    quint16 getSourcePort() const { return sourcePort; }
    quint16 getDestinationPort() const { return destinationPort; }
    TransportProtocol getTransport() const { return transport; }
};

#endif // FIVETUPLE_HPP
