#pragma once
#include <string>
#include <vector>
namespace NetworkMiner { namespace ToolInterfaces { class IDataExporter { public: virtual ~IDataExporter() = default; }; class IHostExporter : public IDataExporter { public: virtual ~IHostExporter() = default; }; } }