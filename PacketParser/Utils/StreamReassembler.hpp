#pragma once

#include <QByteArray>
#include <QBuffer>
#include <QString>

namespace PacketParser {
namespace Utils {

class StreamReassembler {
private:
    QByteArray DATA_TERMINATOR;
    QBuffer dataStream;
    bool terminatorFound;
    int postTerminatorSkipBytes;

public:
    StreamReassembler(const QByteArray& terminator, int postTerminatorSkipBytes);
    ~StreamReassembler();

    QBuffer& DataStream() { return dataStream; }
    bool TerminatorFound() const { return terminatorFound; }

    int AddData(const QString& dataString);
    int AddData(const QByteArray& buffer, int offset, int count);
    int AddData(const char* buffer, int offset, int count);

    void Close();
};

} // namespace Utils
} // namespace PacketParser
