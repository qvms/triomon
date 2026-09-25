#pragma once
#include <string>
namespace NetworkMiner { namespace ToolInterfaces { class IDomainNameFilter { public: virtual ~IDomainNameFilter() = default; virtual bool ContainsDomain(const std::string& domainName, std::string& queriedDomainName) = 0; }; } }