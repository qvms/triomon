#pragma once
#include "IPcapParserFactory.hpp"
#include "IPcapStreamReader.hpp"
#include "IPcapParser.hpp"
#include "PcapFileReader.hpp"
#include <vector>
#include <string>
#include <memory>

namespace SharedUtils {
namespace Pcap {

class PcapParserFactory : public IPcapParserFactory {
public:
    std::vector<std::string> get_SupportedExtensions() const override {
        return {"pcap", "cap", "dump", "dmp", "eth", "log", "etl"};
    }

    std::shared_ptr<IPcapParser> CreatePcapParser(std::shared_ptr<IPcapStreamReader> pcapStreamReader) {
        return nullptr; // Stubbed or simplified for now, as IPcapParser implementation might be complex
    }
};

}
}
