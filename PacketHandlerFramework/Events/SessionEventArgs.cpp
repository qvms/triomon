#include "SessionEventArgs.hpp"

namespace PacketHandlerFramework {
namespace Events {

SessionEventArgs::SessionEventArgs(NetworkFlow* flow, PacketParser::ApplicationLayerProtocol protocol, long long startFrameNumber)
    : Protocol(protocol), StartFrameNumber(startFrameNumber), flow(flow) {
    if (flow && flow->FiveTupleObj) {
        this->Client = flow->FiveTupleObj->ClientHost;
        this->Server = flow->FiveTupleObj->ServerHost;
        this->ClientPort = flow->FiveTupleObj->ClientPort;
        this->ServerPort = flow->FiveTupleObj->ServerPort;
        this->Tcp = (flow->FiveTupleObj->Transport == FiveTuple::TransportProtocol::TCP);
        this->StartTimestamp = flow->StartTime;
    }
}

}
}
