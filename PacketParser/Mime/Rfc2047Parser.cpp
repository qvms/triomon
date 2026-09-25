#include "Rfc2047Parser.hpp"
#include <QStringList>
#include <QByteArray>
#include <QTextCodec>
#include <stdexcept>
#include <vector>

namespace PacketParser {
namespace Mime {

namespace {
    QByteArray ReadQuotedPrintable(const QByteArray& input) {
        // Basic Quoted-Printable decoder
        QByteArray output;
        for (int i = 0; i < input.size(); ++i) {
            if (input[i] == '=' && i + 2 < input.size()) {
                QByteArray hex = input.mid(i + 1, 2);
                bool ok;
                char c = hex.toInt(&ok, 16);
                if (ok) {
                    output.append(c);
                    i += 2;
                } else {
                    output.append(input[i]);
                }
            } else if (input[i] == '_') {
                output.append(' ');
            } else {
                output.append(input[i]);
            }
        }
        return output;
    }
}

QString Rfc2047Parser::DecodeRfc2047Parts(const QString& encoded) {
    try {
        QString decoded;
        int offset = 0;
        
        while (offset < encoded.length() - 4) {
            int start = encoded.indexOf("=?", offset);
            
            if (start < 0) { // no more RFC 2047 string
                decoded.append(encoded.mid(offset));
                return decoded;
            } else if (offset > start) {
                // Log warning (stubbed)
            } else if (start > offset) {
                decoded.append(encoded.mid(offset, start - offset));
            }
            
            int end = encoded.indexOf("?=", start + 2);
            while (end > start && end <= encoded.length() - 2) {
                if (IsRfc2047String(encoded.mid(start, end - start + 2))) {
                    break;
                } else {
                    end = encoded.indexOf("?=", end + 1);
                }
            }
            
            if (end < 0) {
                decoded.append(encoded.mid(start));
                return decoded;
            } else if (IsRfc2047String(encoded.mid(start, end - start + 2))) {
                end += 2;
                decoded.append(ParseRfc2047String(encoded.mid(start, end - start)));
                offset = end;
            } else {
                decoded.append(encoded.mid(start));
                return decoded;
            }
        }
        
        if (offset < encoded.length()) {
            decoded.append(encoded.mid(offset));
        }
        
        return decoded;
    } catch (...) {
        // Log warning (stubbed)
        return encoded;
    }
}

bool Rfc2047Parser::IsRfc2047String(const QString& s) {
    if (s.isEmpty() || s.length() < 1) {
        return false;
    }
    try {
        if (!s.startsWith("=?") || !s.endsWith("?=")) {
            return false;
        }
        
        QString trimmed = s.trimmed();
        while(trimmed.startsWith('=')) trimmed = trimmed.mid(1);
        while(trimmed.endsWith('=')) trimmed = trimmed.chopped(1);
        
        QStringList parts = trimmed.split('?', Qt::SkipEmptyParts);
        return parts.length() == 3;
    } catch (...) {
        return false;
    }
}

QString Rfc2047Parser::ParseRfc2047String(const QString& rfc2047String) {
    if (rfc2047String.startsWith("=?") && rfc2047String.endsWith("?=")) {
        QString trimmed = rfc2047String.trimmed();
        while(trimmed.startsWith('=')) trimmed = trimmed.mid(1);
        while(trimmed.endsWith('=')) trimmed = trimmed.chopped(1);
        
        QStringList parts = trimmed.split('?', Qt::SkipEmptyParts);
        
        if (parts.length() == 3) {
            QByteArray charset = parts[0].toLatin1();
            QTextCodec* codec = QTextCodec::codecForName(charset);
            if (!codec) {
                codec = QTextCodec::codecForName("UTF-8"); // Fallback
            }
            
            QByteArray bytes;
            QString dataStr = parts[2];
            for (int i = 0; i < dataStr.length(); i++) {
                bytes.append(static_cast<char>(dataStr[i].unicode() & 0xFF));
            }
            
            QString encodingStr = parts[1];
            if (encodingStr.compare("B", Qt::CaseInsensitive) == 0) {
                bytes = QByteArray::fromBase64(bytes);
            } else if (encodingStr.compare("Q", Qt::CaseInsensitive) == 0) {
                bytes = ReadQuotedPrintable(bytes);
            }
            
            return codec->toUnicode(bytes);
        } else {
            throw std::runtime_error("Invalid RFC 2047 string");
        }
    } else {
        throw std::runtime_error("Invalid RFC 2047 string");
    }
}

} // namespace Mime
} // namespace PacketParser
