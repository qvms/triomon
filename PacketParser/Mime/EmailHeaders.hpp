#pragma once

#include <string>
#include <vector>

namespace PacketParser {
namespace Mime {

class EmailHeaders {
public:
    static const std::string HEADER_FROM;
    static const std::string HEADER_TO;
    static const std::string HEADER_SUBJECT;
    static const std::string HEADER_MESSAGE_ID;
    static const std::string HEADER_DATE;

    static const std::string HEADER_CONTENT_TRANSFER_ENCODING;
    static const std::string HEADER_CONTENT_TYPE;
    static const std::string HEADER_MIME_VERSION;
    static const std::string HEADER_RETURN_PATH;
    static const std::string HEADER_DELIVERED_TO;
    static const std::string HEADER_RECEIVED;

    static const std::vector<std::string> COMMON_HEADERS;
};

} // namespace Mime
} // namespace PacketParser
