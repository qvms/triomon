#ifndef PCAPFILEWRITER_HPP
#define PCAPFILEWRITER_HPP

#include <QString>
#include <QFile>
#include <QDateTime>
#include "IPcapFrame.hpp"
#include "../../PacketParser/PcapFrameDataLinkTypeEnum.hpp"

class PcapFileWriter {
public:
    PcapFileWriter(const QString& filename, PcapFrameDataLinkTypeEnum dataLinkType);
    ~PcapFileWriter();

    bool getIsOpen() const { return isOpen; }
    QString getFilename() const { return filename; }

    void writeFrame(const IPcapFrame* frame);
    void close();

    static QByteArray toByteArray(qint64 value);
    static QByteArray toByteArray(quint32 value);
    static QByteArray toByteArray(quint16 value);
    static void toByteArray(quint16 value, QByteArray& array, int arrayOffset);
    static void toByteArray(quint32 value, QByteArray& array, int arrayOffset);

private:
    QFile fileStream;
    static const quint16 MAJOR_VERSION_NUMBER = 0x02;
    static const quint16 MINOR_VERSION_NUMBER = 0x04;
    static const quint32 MAGIC_NUMBER = 0xa1b2c3d4;
    QDateTime referenceTime;
    bool isOpen;
    QString filename;
};

#endif // PCAPFILEWRITER_HPP
