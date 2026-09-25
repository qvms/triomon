#pragma once
#include <QString>

namespace PacketParser {
namespace Mime {

class Rfc2047Parser {
public:
    static QString DecodeRfc2047Parts(const QString& encoded);
    static bool IsRfc2047String(const QString& s);
    static QString ParseRfc2047String(const QString& rfc2047String);
};

} // namespace Mime
} // namespace PacketParser
