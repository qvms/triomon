#pragma once
#include <string>
#include <map>
#include <chrono>
#include "../../PacketParser/ApplicationLayerProtocol.hpp"

namespace PacketHandlerFramework {
    class NetworkHost;

namespace Events {

class MessageEventArgs {
public:
    PacketParser::ApplicationLayerProtocol Protocol;
    NetworkHost* SourceHost;
    NetworkHost* DestinationHost;
    long long StartFrameNumber;
    std::chrono::system_clock::time_point StartTimestamp;

    std::string From;
    std::string To;
    std::string Subject;
    std::string Message;
    std::map<std::string, std::string> Attributes;
    long long Size;

    MessageEventArgs(PacketParser::ApplicationLayerProtocol protocol, NetworkHost* sourceHost, NetworkHost* destinationHost, long long startFrameNumber, std::chrono::system_clock::time_point startTimestamp, const std::string& from, const std::string& to, const std::string& subject, const std::string& message, const std::map<std::string, std::string>& attributes, long long size)
        : Protocol(protocol), SourceHost(sourceHost), DestinationHost(destinationHost), StartFrameNumber(startFrameNumber), StartTimestamp(startTimestamp), From(from), To(to), Subject(subject), Message(message), Attributes(attributes), Size(size) {}
};

}
}