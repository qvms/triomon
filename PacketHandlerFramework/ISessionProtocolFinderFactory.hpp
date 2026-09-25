#ifndef ISESSIONPROTOCOLFINDERFACTORY_HPP
#define ISESSIONPROTOCOLFINDERFACTORY_HPP

class PacketHandler;
class NetworkFlow;
class ISessionProtocolFinder;

class ISessionProtocolFinderFactory {
public:
    virtual ~ISessionProtocolFinderFactory() = default;

    virtual PacketHandler* getPacketHandler() const = 0;
    virtual void setPacketHandler(PacketHandler* handler) = 0;

    virtual bool getPortIndependentProtocolIdentificationEnabled() const = 0;
    virtual void setPortIndependentProtocolIdentificationEnabled(bool enabled) = 0;

    virtual ISessionProtocolFinder* createProtocolFinder(NetworkFlow* flow, qint64 startFrameNumber) = 0;
    virtual void reset() = 0;
};

#endif // ISESSIONPROTOCOLFINDERFACTORY_HPP
