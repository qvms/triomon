#include "EmailHeaders.hpp"

namespace PacketParser {
namespace Mime {

const std::string EmailHeaders::HEADER_FROM = "From";
const std::string EmailHeaders::HEADER_TO = "To";
const std::string EmailHeaders::HEADER_SUBJECT = "Subject";
const std::string EmailHeaders::HEADER_MESSAGE_ID = "Message-ID";
const std::string EmailHeaders::HEADER_DATE = "Date";

const std::string EmailHeaders::HEADER_CONTENT_TRANSFER_ENCODING = "Content-Transfer-Encoding";
const std::string EmailHeaders::HEADER_CONTENT_TYPE = "Content-Type";
const std::string EmailHeaders::HEADER_MIME_VERSION = "MIME-Version";
const std::string EmailHeaders::HEADER_RETURN_PATH = "Return-Path";
const std::string EmailHeaders::HEADER_DELIVERED_TO = "Delivered-To";
const std::string EmailHeaders::HEADER_RECEIVED = "Received";

const std::vector<std::string> EmailHeaders::COMMON_HEADERS = {
    HEADER_FROM,
    HEADER_TO,
    HEADER_SUBJECT,
    HEADER_MESSAGE_ID,
    HEADER_DATE,
    HEADER_CONTENT_TRANSFER_ENCODING,
    HEADER_CONTENT_TYPE,
    HEADER_MIME_VERSION,
    HEADER_RETURN_PATH,
    HEADER_DELIVERED_TO,
    HEADER_RECEIVED
};

} // namespace Mime
} // namespace PacketParser
