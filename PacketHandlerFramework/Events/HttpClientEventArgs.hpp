#pragma once

namespace PacketHandlerFramework {
    class NetworkHost;
namespace Events {

class HttpClientEventArgs { 
public:
    NetworkHost* Host;
    HttpClientEventArgs(NetworkHost* host) : Host(host) {} 
};

class NetworkHostEventArgs {
public:
    NetworkHost* Host;
    NetworkHostEventArgs(NetworkHost* host) : Host(host) {}
};

}
}