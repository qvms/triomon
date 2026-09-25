#pragma once

#include <cstdint>

namespace PacketParser {

class ITcpFlowInfo {
public:
    virtual ~ITcpFlowInfo() = default;

    virtual uint16_t getClientPort() const = 0;
    virtual uint16_t getServerPort() const = 0;

    virtual int64_t getBytesSentClient() const = 0;
    virtual void setBytesSentClient(int64_t bytes) = 0;

    virtual int64_t getBytesSentServer() const = 0;
    virtual void setBytesSentServer(int64_t bytes) = 0;
};

} // namespace PacketParser