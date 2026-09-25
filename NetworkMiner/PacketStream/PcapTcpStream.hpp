#pragma once

#include <QObject>
#include <QIODevice>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include <functional>

namespace NetworkMiner {
namespace PacketStream {

class PcapTcpStream : public QObject {
    Q_OBJECT
public:
    enum class TcpSocketState { Listening, Connected, Receiving, Closed };

    PcapTcpStream(quint16 localTcpPort, bool useSsl, int idleTimeoutMilliSeconds, QObject* parent = nullptr);
    PcapTcpStream(const QString& remoteIpOrHost, quint16 remotePort, bool useSsl, int idleTimeoutMilliSeconds, std::function<void()> streamEstablishedHandler, QObject* parent = nullptr);
    ~PcapTcpStream();

    QIODevice* PcapStream() const { return pcapStream; }
    QHostAddress RemoteIP() const { return remoteIP; }
    TcpSocketState SocketState() const { return socketState; }
    void setSocketState(TcpSocketState state) { socketState = state; }
    int IdleTimeoutMilliSeconds() const { return idleTimeoutMS; }

    bool IsClosed() const;
    void BeginAcceptTcpClient(std::function<void()> streamEstablishedHandler);
    void BlockingWaitForTcpClient();
    void Dispose();

private slots:
    void AcceptTcpClientCallback();

private:
    PcapTcpStream(bool useSsl, int idleTimeoutMilliSeconds, QObject* parent = nullptr);
    void SetSocketAsConnected();

    QIODevice* pcapStream;
    QHostAddress remoteIP;
    TcpSocketState socketState;
    QTcpServer* tcpListener;
    bool useSsl;
    QTcpSocket* tcpClient;
    int idleTimeoutMS;
    std::function<void()> m_streamEstablishedHandler;
};

} // namespace PacketStream
} // namespace NetworkMiner
