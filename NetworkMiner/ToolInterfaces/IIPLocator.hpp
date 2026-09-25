#pragma once
#include <string>
namespace NetworkMiner { namespace ToolInterfaces { class IIPLocator { public: virtual ~IIPLocator() = default; virtual std::string GetCountry(const std::string& ipAddress) = 0; }; } }