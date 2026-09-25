#pragma once

namespace PacketParser {
namespace Events {

class BufferUsageEventArgs {
public:
    int BufferUsagePercent;
    BufferUsageEventArgs(int bufferUsagePercent) : BufferUsagePercent(bufferUsagePercent) {}
};

}
}
