#pragma once
#include <vector>
#include <string>
namespace SharedUtils { namespace Pcap { class IPcapParserFactory { public: virtual ~IPcapParserFactory() = default; virtual std::vector<std::string> get_SupportedExtensions() const = 0; }; } }