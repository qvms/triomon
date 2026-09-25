#pragma once
#include <string>
namespace SharedUtils { namespace Pcap { class IFrameWriter { public: virtual ~IFrameWriter() = default; virtual bool get_IsOpen() const = 0; virtual std::string get_Filename() const = 0; virtual bool get_OutputIsPcapNg() const = 0; virtual void Close() = 0; }; } }