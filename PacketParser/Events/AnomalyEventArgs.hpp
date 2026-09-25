#pragma once
#include <string>

namespace PacketParser {
namespace Events {

class AnomalyEventArgs {
public:
    std::string Message;
    AnomalyEventArgs(const std::string& anomalyMessage) : Message(anomalyMessage) {}
};

}
}
