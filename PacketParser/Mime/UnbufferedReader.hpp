#ifndef UNBUFFEREDREADER_HPP
#define UNBUFFEREDREADER_HPP

#include <QIODevice>
#include <QString>
#include <QByteArray>
#include <QList>
#include <vector>

class UnbufferedReader {
public:
    UnbufferedReader(QIODevice* stream);

    QIODevice* getBaseStream() const { return stream; }
    bool getEndOfStream() const { return stream->atEnd(); }

    QString readLine(int returnStringTruncateLength);
    qint64 readTo(const QByteArray& pattern, QByteArray& readBytes);

private:
    std::vector<int> kmpFailureFunction(const QByteArray& pattern);
    
    QIODevice* stream;
};

#endif // UNBUFFEREDREADER_HPP
