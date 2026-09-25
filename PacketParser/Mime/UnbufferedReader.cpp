#include "UnbufferedReader.hpp"

UnbufferedReader::UnbufferedReader(QIODevice* stream) : stream(stream) {
    if (stream) {
        stream->seek(0);
    }
}

QString UnbufferedReader::readLine(int returnStringTruncateLength) {
    QByteArray lineBreak;
    lineBreak.append(static_cast<char>(0x0d)).append(static_cast<char>(0x0a));
    QByteArray lineBytes;
    
    qint64 breakPosition = readTo(lineBreak, lineBytes);
    if (lineBytes.size() < 2 || breakPosition < 0) {
        return QString();
    } else {
        QString result;
        for (int i = 0; i < lineBytes.size() - 2 && result.length() < returnStringTruncateLength; ++i) {
            char c = lineBytes.at(i);
            if (c >= 0x20 || c == '\t') { // not control char
                result.append(c);
            }
        }
        return result;
    }
}

qint64 UnbufferedReader::readTo(const QByteArray& pattern, QByteArray& readBytes) {
    readBytes.clear();
    if (!stream || pattern.isEmpty()) return -1;
    
    std::vector<int> f = kmpFailureFunction(pattern);
    int j = 0;
    
    char c;
    if (!stream->getChar(&c)) return -1;
    
    readBytes.append(c);
    
    while (stream->pos() <= stream->size()) {
        if (pattern.at(j) == c) {
            if (j == pattern.size() - 1) {
                return stream->pos() - pattern.size();
            }
            if (!stream->getChar(&c)) return -1;
            readBytes.append(c);
            j++;
        } else if (j > 0) {
            j = f[j - 1];
        } else {
            if (!stream->getChar(&c)) return -1;
            readBytes.append(c);
        }
    }
    
    return -1;
}

std::vector<int> UnbufferedReader::kmpFailureFunction(const QByteArray& pattern) {
    std::vector<int> f(pattern.size(), 0);
    int i = 1;
    int j = 0;
    
    while (i < pattern.size()) {
        if (pattern.at(j) == pattern.at(i)) {
            f[i] = j + 1;
            i++;
            j++;
        } else if (j > 0) {
            j = f[j - 1];
        } else {
            f[i] = 0;
            i++;
        }
    }
    return f;
}
