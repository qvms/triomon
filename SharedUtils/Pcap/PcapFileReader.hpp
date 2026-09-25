#ifndef PCAPFILEREADER_HPP
#define PCAPFILEREADER_HPP

#include <QString>
#include <QFile>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QQueue>
#include "IPcapFrame.hpp"
#include "../../PacketParser/PcapFrameDataLinkTypeEnum.hpp"

class PcapFileReader : public QThread {
    Q_OBJECT
public:
    PcapFileReader(const QString& filename);
    ~PcapFileReader();

    void abortFileRead();
    int getPercentRead() const;
    int getPacketBytesInQueue() const;
    QString getFilename() const { return filename; }
    PcapFrameDataLinkTypeEnum getFileDataLinkType() const { return dataLinkType; }

    bool hasNextPacket();
    IPcapFrame* getNextPacket();

protected:
    void run() override;

private:
    IPcapFrame* readPcapPacket();
    quint16 toUInt16(const QByteArray& buffer, bool littleEndian) const;
    quint32 toUInt32(const QByteArray& buffer, bool littleEndian) const;

    QString filename;
    QFile fileStream;
    bool littleEndian;
    quint16 majorVersionNumber;
    quint16 minorVersionNumber;
    int timezoneOffsetSeconds;
    quint32 maximumPacketSize;
    PcapFrameDataLinkTypeEnum dataLinkType;

    QQueue<IPcapFrame*> packetQueue;
    mutable QMutex queueMutex;
    QWaitCondition queueNotFull;
    QWaitCondition queueNotEmpty;
    
    int enqueuedByteCount;
    int dequeuedByteCount;
    bool abortRequested;

    static const int PACKET_QUEUE_SIZE = 1000;
};

#endif // PCAPFILEREADER_HPP
