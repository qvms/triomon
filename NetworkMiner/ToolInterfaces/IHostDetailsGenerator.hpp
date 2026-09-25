#pragma once
#include <string>
namespace NetworkMiner { namespace ToolInterfaces { class IHostDetailsGenerator { public: virtual ~IHostDetailsGenerator() = default; virtual void DownloadDatabase() = 0; virtual std::string GetDefaultKeyName() = 0; virtual std::string GetVersionString() = 0; }; } }