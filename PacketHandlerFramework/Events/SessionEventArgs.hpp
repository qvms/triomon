#pragma once
#include <chrono>
#include "../../PacketParser/ApplicationLayerProtocol.hpp"
#include "../FiveTuple.hpp"

namespace PacketHandlerFramework {
    class NetworkHost;
    class NetworkFlow {
    public:
        FiveTuple* FiveTupleObj; // Need pointer/reference as we don't know if FiveTuple is copyable yet though it is.
        std::chrono::system_clock::time_point StartTime;
    };

namespace Events {

class SessionEventArgs {
public:
    PacketParser::ApplicationLayerProtocol Protocol;
    QString Client;
    QString Server;
    uint16_t ClientPort;
    uint16_t ServerPort;
    bool Tcp;
    long long StartFrameNumber;
    std::chrono::system_clock::time_point StartTimestamp;

private:
    NetworkFlow* flow;

public:
    SessionEventArgs(NetworkFlow* flow, PacketParser::ApplicationLayerProtocol protocol, long long startFrameNumber);
    NetworkFlow* GetFlow() const { return flow; }
};

}
}