#pragma once
#include <vector>
#include <cstdint>
namespace SharedUtils { namespace Pcap { class IPcapStreamReader { public: virtual ~IPcapStreamReader() = default; virtual long get_Position() const = 0; virtual std::vector<uint8_t> BlockingRead(int bytesToRead) = 0; virtual int BlockingRead(std::vector<uint8_t>& buffer, int offset, int count) = 0; }; } }