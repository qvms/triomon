#pragma once

#include <queue>
#include <memory>
#include "Frame.hpp" // Placeholder for Frame include

namespace PacketParser {

class LatestFramesQueue {
private:
    std::queue<std::shared_ptr<Frame>> queue;
    int maxSize;

public:
    explicit LatestFramesQueue(int maxNoFrames);

    int MaxSize() const { return maxSize; }
    int Count() const { return static_cast<int>(queue.size()); }

    void Enqueue(std::shared_ptr<Frame> frame);
};

} // namespace PacketParser
