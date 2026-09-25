#pragma once
#include <string>

namespace PacketParser {
    class Frame;
    class NetworkHost;
namespace Events {

class KeywordEventArgs {
public:
    Frame* FrameObj;
    int KeywordIndex;
    int KeywordLength;
    NetworkHost* SourceHost;
    NetworkHost* DestinationHost;
    std::string SourcePort;
    std::string DestinationPort;

    KeywordEventArgs(Frame* frame, int keywordIndex, int keywordLength, NetworkHost* sourceHost, NetworkHost* destinationHost, const std::string& sourcePort, const std::string& destinationPort)
        : FrameObj(frame), KeywordIndex(keywordIndex), KeywordLength(keywordLength), SourceHost(sourceHost), DestinationHost(destinationHost), SourcePort(sourcePort), DestinationPort(destinationPort) {}
};

}
}