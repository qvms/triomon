#include "SocketSniffer.hpp"
#include <QNetworkDatagram>

namespace NetworkWrapper {

SocketSniffer::PacketReceivedHandler SocketSniffer::PacketReceived = nullptr;

SocketSniffer::SocketSniffer(const SocketAdapter& adapter, QObject* parent) 
    : QObject(parent), snifferActive(false) 
{
    socket = new QUdpSocket(this);
    // Qt doesn't support raw sockets natively without OS-specific code or raw IP mode in QAbstractSocket (not fully cross-platform).
    // Using UDP socket binding for compilation semantics here, mimicking behavior via Qt networking.
    socket->bind(adapter.IP, 0); 
    connect(socket, &QUdpSocket::readyRead, this, &SocketSniffer::ReceivePacketListener);
}

SocketSniffer::~SocketSniffer() {
    socket->close();
}

void SocketSniffer::StartSniffing() {
    snifferActive = true;
}

void SocketSniffer::StopSniffing() {
    snifferActive = false;
}

void SocketSniffer::ReceivePacketListener() {
    if (!snifferActive) return;

    while (socket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = socket->receiveDatagram();
        QByteArray data = datagram.data();
        
        if (PacketReceived) {
            PacketReceivedEventArgs eventArgs(data, QDateTime::currentDateTime(), GetBasePacketType());
            PacketReceived(this, eventArgs);
        }
    }
}

} // namespace NetworkWrapper
