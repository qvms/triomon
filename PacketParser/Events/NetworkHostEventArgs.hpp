#pragma once

namespace PacketParser {
    class NetworkHost;
namespace Events {

class NetworkHostEventArgs {
public:
    NetworkHost* Host;
    NetworkHostEventArgs(NetworkHost* host) : Host(host) {}
};

}
}