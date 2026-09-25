#include "PartBuilder.hpp"
#include "ByteArrayStream.hpp"

QList<MultipartPart> PartBuilder::getParts(const QByteArray& mimeMultipartData, const QString& boundary) {
    ByteArrayStream stream(mimeMultipartData);
    UnbufferedReader reader(&stream);
    return getParts(&reader, boundary);
}

QList<MultipartPart> PartBuilder::getParts(UnbufferedReader* streamReader, const QString& boundary) {
    QList<MultipartPart> parts;
    if (!streamReader || !streamReader->getBaseStream()) return parts;

    QString interPartBoundary = "--" + boundary;
    QString finalBoundary = "--" + boundary + "--";
    QIODevice* stream = streamReader->getBaseStream();

    while (!streamReader->getEndOfStream()) {
        qint64 partStartPosition = stream->pos();
        int partLength = 0;
        QString line = streamReader->readLine(200);

        while (line != interPartBoundary && line != finalBoundary) {
            partLength = static_cast<int>(stream->pos() - 2 - partStartPosition);
            line = streamReader->readLine(200);
            if (line.isNull() || line.isEmpty() && streamReader->getEndOfStream()) {
                return parts; // end of stream
            }
        }
        
        qint64 nextPartStartPosition = stream->pos();

        if (partLength > 0) {
            stream->seek(partStartPosition);
            QByteArray partData = stream->read(partLength);
            parts.append(MultipartPart(partData));
        }
        
        if (line == finalBoundary) {
            break;
        } else {
            stream->seek(nextPartStartPosition);
        }
    }

    return parts;
}
