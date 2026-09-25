#include "LpdPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

LpdPacketHandler::LpdPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> LpdPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::LpdPacket>() };
}

ApplicationLayerProtocol LpdPacketHandler::HandledProtocol() const {
    return ApplicationLayerProtocol::Lpd;
}

int LpdPacketHandler::ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    int bytesHandled = 0;
    for (const auto& p : packetList) {
        if (auto lpdPacket = std::dynamic_pointer_cast<PacketParser::Packets::LpdPacket>(p)) {
            bytesHandled += lpdPacket->ParsedBytesCount();
            if (lpdPacket->MessageCode() == 0x02 || lpdPacket->MessageCode() == 0x03) {
                QString filename = lpdPacket->Operand();
                if (filename.isEmpty()) {
                    filename = "lpd.data";
                }

                auto assembler = std::make_shared<NetworkMiner::FileStreamAssembler>(
                    MainPacketHandler()->FileStreamAssemblerList(),
                    tcpSession->GetFlow()->FiveTuple(),
                    transferIsClientToServer,
                    NetworkMiner::FileStreamTypes::Lpd,
                    filename,
                    QString(),
                    QString(),
                    lpdPacket->ParentFrame()->GetFrameNumber(),
                    lpdPacket->ParentFrame()->GetTimestamp()
                );
                assembler->SetFileContentLength(lpdPacket->PacketLength() - lpdPacket->ParsedBytesCount());
                assembler->SetFileSegmentRemainingBytes(lpdPacket->PacketLength() - lpdPacket->ParsedBytesCount());
                assemblerList.insert(tcpSession, assembler);
                MainPacketHandler()->FileStreamAssemblerList()->Add(assembler);
            }
        }
    }
    return bytesHandled;
}

void LpdPacketHandler::Reset() {
    assemblerList.clear();
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
