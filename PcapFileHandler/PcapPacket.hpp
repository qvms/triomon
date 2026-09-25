#pragma once
#include <QDateTime>
#include <vector>
#include <cstdint>

namespace PcapFileHandler {

class PcapPacket {
private:
    QDateTime timestamp;
    std::vector<uint8_t> data;

public:
    PcapPacket(const QDateTime& timestamp, const std::vector<uint8_t>& data)
        : timestamp(timestamp), data(data) {
    }

    QDateTime get_Timestamp() const {
        return timestamp;
    }

    std::vector<uint8_t> get_Data() const {
        return data;
    }
};

} // namespace PcapFileHandler
