#include "PcapFileReader.hpp"
#include "PcapFrame.hpp"
#include <QDateTime>
#include <QDataStream>

PcapFileReader::PcapFileReader(const QString& filename)
    : filename(filename),
      fileStream(filename),
      littleEndian(false),
      majorVersionNumber(0),
      minorVersionNumber(0),
      timezoneOffsetSeconds(0),
      maximumPacketSize(0),
      dataLinkType(PcapFrameDataLinkTypeEnum::WTAP_ENCAP_ETHERNET),
      enqueuedByteCount(0),
      dequeuedByteCount(0),
      abortRequested(false)
{
    if (!fileStream.open(QIODevice::ReadOnly)) {
        throw std::runtime_event("Cannot open file");
    }

    QByteArray buffer4 = fileStream.read(4);
    quint32 wiresharkMagicNumber = 0xa1b2c3d4;

    if (wiresharkMagicNumber == toUInt32(buffer4, false)) {
        littleEndian = false;
    } else if (wiresharkMagicNumber == toUInt32(buffer4, true)) {
        littleEndian = true;
    } else {
        throw std::runtime_event("Not a PCAP file.");
    }

    majorVersionNumber = toUInt16(fileStream.read(2), littleEndian);
    minorVersionNumber = toUInt16(fileStream.read(2), littleEndian);
    timezoneOffsetSeconds = static_cast<int>(toUInt32(fileStream.read(4), littleEndian));
    fileStream.read(4); // accuracy of timestamps
    maximumPacketSize = toUInt32(fileStream.read(4), littleEndian);
    dataLinkType = static_cast<PcapFrameDataLinkTypeEnum>(toUInt32(fileStream.read(4), littleEndian));

    start(); // Start the background thread
}

PcapFileReader::~PcapFileReader() {
    abortFileRead();
    wait();
    
    QMutexLocker lock(&queueMutex);
    while(!packetQueue.isEmpty()) {
        delete packetQueue.dequeue();
    }
}

void PcapFileReader::abortFileRead() {
    QMutexLocker lock(&queueMutex);
    abortRequested = true;
    queueNotFull.wakeAll();
    queueNotEmpty.wakeAll();
}

int PcapFileReader::getPercentRead() const {
    if (fileStream.size() == 0) return 0;
    return static_cast<int>(((fileStream.pos() - getPacketBytesInQueue()) * 100) / fileStream.size());
}

int PcapFileReader::getPacketBytesInQueue() const {
    return enqueuedByteCount - dequeuedByteCount;
}

void PcapFileReader::run() {
    while (!abortRequested) {
        bool shouldRead = false;
        {
            QMutexLocker lock(&queueMutex);
            if (packetQueue.size() >= PACKET_QUEUE_SIZE) {
                queueNotFull.wait(&queueMutex, 100);
            }
            shouldRead = packetQueue.size() < PACKET_QUEUE_SIZE && !fileStream.atEnd();
        }

        if (abortRequested) break;

        if (shouldRead) {
            IPcapFrame* packet = readPcapPacket();
            if (packet) {
                QMutexLocker lock(&queueMutex);
                packetQueue.enqueue(packet);
                enqueuedByteCount += packet->getData().size();
                queueNotEmpty.wakeOne();
            } else {
                break; // EOF or error
            }
        }
    }
}

bool PcapFileReader::hasNextPacket() {
    QMutexLocker lock(&queueMutex);
    if (!packetQueue.isEmpty()) return true;
    if (fileStream.atEnd() && packetQueue.isEmpty()) return false;
    
    // Wait briefly
    queueNotEmpty.wait(&queueMutex, 100);
    return !packetQueue.isEmpty() || !fileStream.atEnd();
}

IPcapFrame* PcapFileReader::getNextPacket() {
    QMutexLocker lock(&queueMutex);
    while (packetQueue.isEmpty() && !fileStream.atEnd() && !abortRequested) {
        queueNotEmpty.wait(&queueMutex, 100);
    }
    
    if (packetQueue.isEmpty()) return nullptr;
    
    IPcapFrame* packet = packetQueue.dequeue();
    dequeuedByteCount += packet->getData().size();
    queueNotFull.wakeOne();
    return packet;
}

IPcapFrame* PcapFileReader::readPcapPacket() {
    if (fileStream.atEnd()) return nullptr;

    QByteArray buffer4 = fileStream.read(4);
    if (buffer4.size() < 4) return nullptr;
    
    quint32 seconds = toUInt32(buffer4, littleEndian);
    quint32 microseconds = toUInt32(fileStream.read(4), littleEndian);
    quint32 bytesToRead = toUInt32(fileStream.read(4), littleEndian);
    fileStream.read(4); // actual length
    
    QByteArray data = fileStream.read(bytesToRead);

    QDateTime timestamp;
    timestamp.setTimeSpec(Qt::UTC);
    timestamp = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(seconds) * 1000 + microseconds / 1000);
    
    return new PcapFrame(timestamp, data, 0); // Need to adjust this constructor based on PcapFrame signature
}

quint16 PcapFileReader::toUInt16(const QByteArray& buffer, bool littleEndian) const {
    if (buffer.size() < 2) return 0;
    if (littleEndian) {
        return static_cast<quint16>(static_cast<quint8>(buffer[0]) | (static_cast<quint8>(buffer[1]) << 8));
    } else {
        return static_cast<quint16>((static_cast<quint8>(buffer[0]) << 8) | static_cast<quint8>(buffer[1]));
    }
}

quint32 PcapFileReader::toUInt32(const QByteArray& buffer, bool littleEndian) const {
    if (buffer.size() < 4) return 0;
    if (littleEndian) {
        return static_cast<quint32>(static_cast<quint8>(buffer[0]) | (static_cast<quint8>(buffer[1]) << 8) | (static_cast<quint8>(buffer[2]) << 16) | (static_cast<quint8>(buffer[3]) << 24));
    } else {
        return static_cast<quint32>((static_cast<quint8>(buffer[0]) << 24) | (static_cast<quint8>(buffer[1]) << 16) | (static_cast<quint8>(buffer[2]) << 8) | static_cast<quint8>(buffer[3]));
    }
}
