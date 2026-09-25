#ifndef BYTEARRAYSTREAM_HPP
#define BYTEARRAYSTREAM_HPP

#include <QBuffer>
#include <QByteArray>

class ByteArrayStream : public QBuffer {
public:
    ByteArrayStream(QByteArray* byteArray) : QBuffer(byteArray) {
        open(QIODevice::ReadOnly);
    }
    ByteArrayStream(const QByteArray& byteArray) : QBuffer(), data(byteArray) {
        setBuffer(&data);
        open(QIODevice::ReadOnly);
    }
private:
    QByteArray data;
};

#endif // BYTEARRAYSTREAM_HPP
