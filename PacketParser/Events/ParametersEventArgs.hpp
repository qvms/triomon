#pragma once
#include <string>
#include <map>
#include <chrono>

namespace PacketParser {
    class NetworkHost;
namespace Events {

class ParametersEventArgs {
public:
    int FrameNumber;
    NetworkHost* SourceHost;
    NetworkHost* DestinationHost;
    std::string SourcePort;
    std::string DestinationPort;
    std::map<std::string, std::string> Parameters;
    std::chrono::system_clock::time_point Timestamp;
    std::string Details;

    ParametersEventArgs(int frameNumber, NetworkHost* sourceHost, NetworkHost* destinationHost, const std::string& sourcePort, const std::string& destinationPort, const std::map<std::string, std::string>& parameters, std::chrono::system_clock::time_point timestamp, const std::string& details)
        : FrameNumber(frameNumber), SourceHost(sourceHost), DestinationHost(destinationHost), SourcePort(sourcePort), DestinationPort(destinationPort), Parameters(parameters), Timestamp(timestamp), Details(details) {}
};

}
}