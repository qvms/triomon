#include "LatestFramesQueue.hpp"

namespace PacketParser {

LatestFramesQueue::LatestFramesQueue(int maxNoFrames) : maxSize(maxNoFrames) {}

void LatestFramesQueue::Enqueue(std::shared_ptr<Frame> frame) {
    queue.push(frame);
    if (static_cast<int>(queue.size()) > maxSize) {
        queue.pop();
    }
}

} // namespace PacketParser
