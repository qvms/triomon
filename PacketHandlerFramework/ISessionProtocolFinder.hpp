#ifndef ISESSIONPROTOCOLFINDER_HPP
#define ISESSIONPROTOCOLFINDER_HPP

#include "NetworkFlow.hpp"

class ISessionProtocolFinder {
public:
    virtual ~ISessionProtocolFinder() = default;

    virtual NetworkFlow* getFlow() const = 0;
    virtual qint64 getStartFrameNumber() const = 0;
    
    // Additional methods based on what ISessionProtocolFinder is supposed to do...
};

#endif // ISESSIONPROTOCOLFINDER_HPP
