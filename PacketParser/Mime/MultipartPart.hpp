#ifndef MULTIPARTPART_HPP
#define MULTIPARTPART_HPP

#include <QMap>
#include <QString>
#include <QByteArray>
#include <QIODevice>
#include "UnbufferedReader.hpp"

class MultipartPart {
public:
    MultipartPart(const QMap<QString, QString>& attributes);
    MultipartPart(const QByteArray& partData);
    MultipartPart(QIODevice* stream, qint64 partStartIndex, int partLength);

    QMap<QString, QString> getAttributes() const { return attributes; }
    QByteArray getData() const { return data; }

private:
    QMap<QString, QString> attributes;
    QByteArray data;
};

#endif // MULTIPARTPART_HPP
