#ifndef PORTPROTOCOLFINDERFACTORY_HPP
#define PORTPROTOCOLFINDERFACTORY_HPP

#include "ISessionProtocolFinderFactory.hpp"
#include "../PacketParser/PacketHandler.hpp"

class PortProtocolFinderFactory : public ISessionProtocolFinderFactory {
public:
    PortProtocolFinderFactory(PacketHandler* packetHandler);

    PacketHandler* getPacketHandler() const override { return packetHandler; }
    void setPacketHandler(PacketHandler* handler) override { packetHandler = handler; }

    bool getPortIndependentProtocolIdentificationEnabled() const override;
    void setPortIndependentProtocolIdentificationEnabled(bool enabled) override;

    ISessionProtocolFinder* createProtocolFinder(NetworkFlow* flow, qint64 startFrameNumber) override;
    void reset() override;

private:
    PacketHandler* packetHandler;
};

#endif // PORTPROTOCOLFINDERFACTORY_HPP
