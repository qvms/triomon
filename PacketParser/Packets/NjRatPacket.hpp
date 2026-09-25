#pragma once

#include "AbstractPacket.hpp" // Placeholder

namespace PacketParser {
namespace Packets {

class NjRatPacket : public AbstractPacket { // Conceptually implements ISessionPacket
public:
    enum class FieldEncoding {
        plaintext,
        base64,
        raw
    };

    enum class Plugins {
        rs,
        rsc,
        proc,
        tcp,
        srv,
        fm
    };

    enum class ProcActions {
        k,
        kd,
        re,
        rss
    };

    enum class FileManagerActions {
        dw,
        up,
        cp,
        rn,
        fl,
        rd
    };

    enum class BotMessageType {
        ll,
        li,
        act,
        inf,
        infn,
        CAP,
        pl,
        PLG,
        sc_tilde,
        scPK,
        CH
    };

public:
    NjRatPacket();
    virtual ~NjRatPacket() = default;
};

} // namespace Packets
} // namespace PacketParser
