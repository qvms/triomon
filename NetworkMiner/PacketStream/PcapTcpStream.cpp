#include "PcapTcpStream.hpp"
#include <QSslSocket>
#include <QSslConfiguration>
#include <stdexcept>
// #include "ServerCert.hpp" // Uncomment and configure when ServerCert is ported

namespace NetworkMiner {
namespace PacketStream {

class CustomTcpServer : public QTcpServer {
    bool m_useSsl;
public:
    CustomTcpServer(bool useSsl, QObject* parent = nullptr) : QTcpServer(parent), m_useSsl(useSsl) {}
protected:
    void incomingConnection(qintptr handle) override {
        if (m_useSsl) {
            QSslSocket* sslSocket = new QSslSocket(this);
            sslSocket->setSocketDescriptor(handle);
            addPendingConnection(sslSocket);
        } else {
            QTcpSocket* socket = new QTcpSocket(this);
            socket->setSocketDescriptor(handle);
            addPendingConnection(socket);
        }
    }
};

PcapTcpStream::PcapTcpStream(quint16 localTcpPort, bool useSsl, int idleTimeoutMilliSeconds, QObject* parent)
    : PcapTcpStream(useSsl, idleTimeoutMilliSeconds, parent)
{
    socketState = TcpSocketState::Closed;
    tcpListener = new CustomTcpServer(useSsl, this);
    tcpListener->listen(QHostAddress::Any, localTcpPort);
    socketState = TcpSocketState::Listening;
}

PcapTcpStream::PcapTcpStream(const QString& remoteIpOrHost, quint16 remotePort, bool useSsl, int idleTimeoutMilliSeconds, std::function<void()> streamEstablishedHandler, QObject* parent)
    : PcapTcpStream(useSsl, idleTimeoutMilliSeconds, parent)
{
    m_streamEstablishedHandler = streamEstablishedHandler;
    
    if (useSsl) {
        tcpClient = new QSslSocket(this);
    } else {
        tcpClient = new QTcpSocket(this);
    }
    
    tcpClient->connectToHost(remoteIpOrHost, remotePort);

    if (tcpClient->waitForConnected()) {
        SetSocketAsConnected();
        // streamEstablishedHandler is not dynamically invoked here in the original C# code.
    } else {
        socketState = TcpSocketState::Closed;
    }
}

PcapTcpStream::PcapTcpStream(bool useSsl, int idleTimeoutMilliSeconds, QObject* parent)
    : QObject(parent), pcapStream(nullptr), tcpListener(nullptr), tcpClient(nullptr)
{
    this->useSsl = useSsl;
    this->idleTimeoutMS = idleTimeoutMilliSeconds;
}

PcapTcpStream::~PcapTcpStream() {
    Dispose();
}

bool PcapTcpStream::IsClosed() const {
    return tcpClient == nullptr || tcpClient->state() != QAbstractSocket::ConnectedState;
}

void PcapTcpStream::BeginAcceptTcpClient(std::function<void()> streamEstablishedHandler) {
    m_streamEstablishedHandler = streamEstablishedHandler;
    if (tcpListener) {
        connect(tcpListener, &QTcpServer::newConnection, this, &PcapTcpStream::AcceptTcpClientCallback);
    }
}

void PcapTcpStream::BlockingWaitForTcpClient() {
    if (tcpListener && tcpListener->waitForNewConnection(-1)) {
        tcpClient = tcpListener->nextPendingConnection();
        if (tcpClient) {
            SetSocketAsConnected();
        }
    }
}

void PcapTcpStream::AcceptTcpClientCallback() {
    if (!tcpListener) return;

    try {
        tcpClient = tcpListener->nextPendingConnection();
        if (tcpClient) {
            SetSocketAsConnected();
        } else {
            Dispose();
            return;
        }
    } catch (...) {
        Dispose();
        return;
    }

    if (m_streamEstablishedHandler) {
        m_streamEstablishedHandler();
    }
}

void PcapTcpStream::SetSocketAsConnected() {
    try {
        socketState = TcpSocketState::Connected;
        remoteIP = tcpClient->peerAddress();

        if (useSsl) {
            QSslSocket* sslSocket = qobject_cast<QSslSocket*>(tcpClient);
            if (sslSocket) {
                // If using a specific certificate from ServerCert:
                // QSslConfiguration config = sslSocket->sslConfiguration();
                // config.setLocalCertificate(ServerCert::Instance()->certificate());
                // config.setPrivateKey(ServerCert::Instance()->privateKey());
                // sslSocket->setSslConfiguration(config);

                sslSocket->startServerEncryption();
                // We might want to wait for encrypted state, but this could block the event loop.
                // In C# it blocked, so we'll block here.
                if (!sslSocket->waitForEncrypted(idleTimeoutMS > 0 ? idleTimeoutMS : 30000)) {
                    throw std::runtime_error("SSL encryption failed");
                }
            }
            pcapStream = tcpClient;
        } else {
            pcapStream = tcpClient;
        }

        // QIODevice in Qt doesn't have a ReadTimeout property like System.IO.Stream.
        // The read timeout should be handled where the stream is being read.
        
    } catch (...) {
        Dispose();
    }
}

void PcapTcpStream::Dispose() {
    socketState = TcpSocketState::Closed;
    
    // pcapStream is just a pointer to tcpClient in our case, so we don't delete it separately.
    if (pcapStream) {
        pcapStream = nullptr;
    }

    if (tcpClient) {
        if (tcpClient->isOpen()) {
            tcpClient->close();
        }
        tcpClient->deleteLater();
        tcpClient = nullptr;
    }

    if (tcpListener) {
        if (tcpListener->isListening()) {
            tcpListener->close();
        }
        tcpListener->deleteLater();
        tcpListener = nullptr;
    }
}

} // namespace PacketStream
} // namespace NetworkMiner
