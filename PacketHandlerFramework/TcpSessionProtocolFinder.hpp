#ifndef TCPSESSIONPROTOCOLFINDER_HPP
#define TCPSESSIONPROTOCOLFINDER_HPP

#include "ISessionProtocolFinder.hpp"

class PacketHandler;

class TcpSessionProtocolFinder : public ISessionProtocolFinder {
public:
    TcpSessionProtocolFinder(NetworkFlow* flow, qint64 startFrameNumber, PacketHandler* packetHandler)
        : flow(flow), startFrameNumber(startFrameNumber), packetHandler(packetHandler) {}

    NetworkFlow* getFlow() const override { return flow; }
    qint64 getStartFrameNumber() const override { return startFrameNumber; }

private:
    NetworkFlow* flow;
    qint64 startFrameNumber;
    PacketHandler* packetHandler;
};

#endif // TCPSESSIONPROTOCOLFINDER_HPP
