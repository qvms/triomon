#pragma once

namespace PacketParser {
    class Frame;
namespace Events {

class FrameEventArgs {
public:
    Frame* FrameObj;
    FrameEventArgs(Frame* frame) : FrameObj(frame) {}
};

}
}