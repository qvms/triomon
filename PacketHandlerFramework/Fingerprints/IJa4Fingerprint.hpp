#pragma once
#include <string>
namespace PacketHandlerFramework { namespace Fingerprints { class IJa4Fingerprint { public: virtual ~IJa4Fingerprint() = default; virtual std::string GetJa4() const = 0; }; } }