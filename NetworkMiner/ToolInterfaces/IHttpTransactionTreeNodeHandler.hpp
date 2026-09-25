#pragma once
#include <string>
namespace NetworkMiner { namespace ToolInterfaces { class IHttpTransactionTreeNodeHandler { public: virtual ~IHttpTransactionTreeNodeHandler() = default; virtual bool IsAdvertisment(const std::string& url) = 0; virtual bool IsInternetTracker(const std::string& url) = 0; }; } }