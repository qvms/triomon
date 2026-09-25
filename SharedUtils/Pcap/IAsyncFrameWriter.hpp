#pragma once
#include <string>
#include "IFrameWriter.hpp"
namespace SharedUtils { namespace Pcap { class IAsyncFrameWriter : public IFrameWriter { public: virtual ~IAsyncFrameWriter() = default; }; } }