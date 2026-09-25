#include "PcapFileWriter.hpp"
#include <QDataStream>

PcapFileWriter::PcapFileWriter(const QString& filename, PcapFrameDataLinkTypeEnum dataLinkType)
    : fileStream(filename),
      isOpen(false),
      filename(filename)
{
    referenceTime = QDateTime(QDate(1970, 1, 1), QTime(0, 0, 0), Qt::UTC);
    if (fileStream.open(QIODevice::WriteOnly)) {
        isOpen = true;
        
        QDataStream out(&fileStream);
        out.setByteOrder(QDataStream::LittleEndian); // PCAP typically writes in native, we'll use little endian
        
        out << static_cast<quint32>(MAGIC_NUMBER);
        out << static_cast<quint16>(MAJOR_VERSION_NUMBER);
        out << static_cast<quint16>(MINOR_VERSION_NUMBER);
        out << static_cast<quint32>(0x00); // Time zone offset
        out << static_cast<quint32>(0x00); // accuracy of timestamps
        out << static_cast<quint32>(0xffff); // max length of captured packets
        out << static_cast<quint32>(dataLinkType);
    }
}

PcapFileWriter::~PcapFileWriter() {
    if (isOpen) {
        close();
    }
}

void PcapFileWriter::writeFrame(const IPcapFrame* frame) {
    if (!isOpen || !frame) return;

    qint64 totalMicroseconds = referenceTime.msecsTo(frame->getTimestamp()) * 1000;
    quint32 seconds = static_cast<quint32>(totalMicroseconds / 1000000);
    quint32 microseconds = static_cast<quint32>(totalMicroseconds % 1000000);

    QDataStream out(&fileStream);
    out.setByteOrder(QDataStream::LittleEndian);

    out << seconds;
    out << microseconds;
    out << static_cast<quint32>(frame->getData().size());
    out << static_cast<quint32>(frame->getData().size());
    
    fileStream.write(frame->getData());
}

void PcapFileWriter::close() {
    fileStream.flush();
    fileStream.close();
    isOpen = false;
}

QByteArray PcapFileWriter::toByteArray(qint64 value) {
    QByteArray array(8, 0);
    toByteArray(static_cast<quint32>(value >> 32), array, 0);
    toByteArray(static_cast<quint32>(value), array, 4);
    return array;
}

QByteArray PcapFileWriter::toByteArray(quint32 value) {
    QByteArray array(4, 0);
    toByteArray(value, array, 0);
    return array;
}

QByteArray PcapFileWriter::toByteArray(quint16 value) {
    QByteArray array(2, 0);
    toByteArray(value, array, 0);
    return array;
}

void PcapFileWriter::toByteArray(quint16 value, QByteArray& array, int arrayOffset) {
    if (array.size() < arrayOffset + 2) array.resize(arrayOffset + 2);
    array[arrayOffset] = static_cast<char>(value >> 8);
    array[arrayOffset + 1] = static_cast<char>(value & 0x00ff);
}

void PcapFileWriter::toByteArray(quint32 value, QByteArray& array, int arrayOffset) {
    if (array.size() < arrayOffset + 4) array.resize(arrayOffset + 4);
    array[arrayOffset] = static_cast<char>(value >> 24);
    array[arrayOffset + 1] = static_cast<char>((value >> 16) & 0x000000ff);
    array[arrayOffset + 2] = static_cast<char>((value >> 8) & 0x000000ff);
    array[arrayOffset + 3] = static_cast<char>(value & 0x000000ff);
}
