#include "MultipartPart.hpp"
#include "ByteArrayStream.hpp"
#include <QStringList>

MultipartPart::MultipartPart(const QMap<QString, QString>& attributes)
    : attributes(attributes)
{
}

MultipartPart::MultipartPart(const QByteArray& partData)
{
    ByteArrayStream* stream = new ByteArrayStream(partData);
    *this = MultipartPart(stream, 0, partData.size());
    delete stream;
}

MultipartPart::MultipartPart(QIODevice* stream, qint64 partStartIndex, int partLength) {
    if(stream && stream->isSequential() == false) {
        stream->seek(partStartIndex);
    }
    
    UnbufferedReader streamReader(stream);
    QString line = streamReader.readLine(200);
    
    while(!line.isEmpty() && stream->pos() < partStartIndex + partLength) {
        QStringList headerDataCollection = line.split(';');
        
        if(headerDataCollection.size() > 1 && headerDataCollection[0].toLower().startsWith("content-disposition: ")) {
            for(int i = 1; i < headerDataCollection.size(); ++i) {
                QString part = headerDataCollection[i];
                if(part.contains("=\"") && part.length() > part.indexOf('\"') + 1) {
                    QString parameterName = part.left(part.indexOf('=')).trimmed();
                    QString parameterValue = part.mid(part.indexOf('\"') + 1, part.lastIndexOf('\"') - part.indexOf('\"') - 1);
                    attributes.insert(parameterName, parameterValue);
                }
            }
        } else if(headerDataCollection.size() > 0 && headerDataCollection[0].toLower().startsWith("content-type: ")) {
            // contentType
        }
        
        line = streamReader.readLine(200);
    }
    
    data = stream->read(partLength + partStartIndex - stream->pos());
}
