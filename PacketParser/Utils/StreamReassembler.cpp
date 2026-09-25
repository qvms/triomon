#include "StreamReassembler.hpp"
#include "KnuthMorrisPratt.hpp"
#include <algorithm>
#include <vector>

namespace PacketParser {
namespace Utils {

StreamReassembler::StreamReassembler(const QByteArray& terminator, int postTerminatorSkipBytes)
    : DATA_TERMINATOR(terminator), postTerminatorSkipBytes(postTerminatorSkipBytes), terminatorFound(false) {
    dataStream.open(QIODevice::ReadWrite);
}

StreamReassembler::~StreamReassembler() {
    Close();
}

int StreamReassembler::AddData(const QString& dataString) {
    QByteArray data = dataString.toLatin1();
    return AddData(data, 0, data.length());
}

int StreamReassembler::AddData(const QByteArray& buffer, int offset, int count) {
    return AddData(buffer.constData(), offset, count);
}

int StreamReassembler::AddData(const char* buffer, int offset, int count) {
    std::vector<uint8_t> readBytes;
    std::vector<uint8_t> pattern(DATA_TERMINATOR.begin(), DATA_TERMINATOR.end());
    std::vector<uint8_t> data(buffer, buffer + offset + count);
    
    long long terminatorIndex = KnuthMorrisPratt::ReadTo(pattern, data, offset, readBytes);
    int bytesRead = 0;
    
    if (terminatorIndex == -1 && dataStream.size() > 0) {
        int oldBytesToRead = std::min(DATA_TERMINATOR.length() - 1, static_cast<int>(dataStream.size()));
        QByteArray oldBufferTail;
        oldBufferTail.resize(oldBytesToRead);
        
        dataStream.seek(dataStream.size() - oldBytesToRead);
        int oldBytesRead = dataStream.read(oldBufferTail.data(), oldBytesToRead);
        
        QByteArray tempBuffer;
        tempBuffer.append(oldBufferTail.constData(), oldBytesRead);
        tempBuffer.append(buffer + offset, count);
        
        std::vector<uint8_t> tempData(tempBuffer.begin(), tempBuffer.end());
        long long tempTerminatorIndex = KnuthMorrisPratt::ReadTo(pattern, tempData, 0, readBytes);
        
        if (tempTerminatorIndex >= 0) {
            bytesRead = static_cast<int>(tempTerminatorIndex) - oldBytesRead + DATA_TERMINATOR.length();
            count = static_cast<int>(tempTerminatorIndex) - oldBytesRead + this->postTerminatorSkipBytes;
            this->terminatorFound = true;
        } else {
            bytesRead = count;
        }
    } else if (terminatorIndex >= 0) {
        bytesRead = static_cast<int>(terminatorIndex) - offset + DATA_TERMINATOR.length();
        count = static_cast<int>(terminatorIndex) - offset + this->postTerminatorSkipBytes;
        this->terminatorFound = true;
    } else {
        bytesRead = count;
    }
    
    if (count > 0) {
        dataStream.seek(dataStream.size());
        dataStream.write(buffer + offset, count);
    }
    
    return bytesRead;
}

void StreamReassembler::Close() {
    if (dataStream.isOpen()) {
        dataStream.close();
    }
}

} // namespace Utils
} // namespace PacketParser
