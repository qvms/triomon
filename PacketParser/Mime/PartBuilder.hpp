#ifndef PARTBUILDER_HPP
#define PARTBUILDER_HPP

#include <QByteArray>
#include <QString>
#include <QList>
#include "MultipartPart.hpp"
#include "UnbufferedReader.hpp"

class PartBuilder {
public:
    static QList<MultipartPart> getParts(const QByteArray& mimeMultipartData, const QString& boundary);
    static QList<MultipartPart> getParts(UnbufferedReader* streamReader, const QString& boundary);
};

#endif // PARTBUILDER_HPP
