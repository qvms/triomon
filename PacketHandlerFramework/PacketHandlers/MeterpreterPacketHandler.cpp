#include "MeterpreterPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

MeterpreterPacketHandler::MeterpreterPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> MeterpreterPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::MeterpreterPacket>() };
}

ApplicationLayerProtocol MeterpreterPacketHandler::HandledProtocol() const {
    return ApplicationLayerProtocol::Meterpreter;
}

int MeterpreterPacketHandler::ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    int bytesHandled = 0;
    for (const auto& p : packetList) {
        if (auto meterpreterPacket = std::dynamic_pointer_cast<PacketParser::Packets::MeterpreterPacket>(p)) {
            bytesHandled += meterpreterPacket->ParsedBytesCount();

            if (meterpreterPacket->PayloadLength() > 0) {
                auto assembler = std::make_shared<NetworkMiner::FileStreamAssembler>(
                    MainPacketHandler()->FileStreamAssemblerList(),
                    tcpSession->GetFlow()->FiveTuple(),
                    transferIsClientToServer,
                    NetworkMiner::FileStreamTypes::Meterpreter,
                    "meterpreter.payload",
                    QString(),
                    "PAYLOAD=reverse_tcp LPORT=" + QString::number(tcpSession->ServerTcpPort()),
                    meterpreterPacket->ParentFrame()->GetFrameNumber(),
                    meterpreterPacket->ParentFrame()->GetTimestamp()
                );
                assembler->SetFileContentLength(meterpreterPacket->PayloadLength());
                assembler->SetFileSegmentRemainingBytes(meterpreterPacket->PayloadLength());
                
                fileStreamAssemblers.insert(tcpSession->GetFlow()->FiveTuple(), assembler);
                MainPacketHandler()->FileStreamAssemblerList()->Add(assembler);
            } else if (meterpreterPacket->HasMZHeader()) {
                if (fileStreamAssemblers.contains(tcpSession->GetFlow()->FiveTuple())) {
                    auto assembler = fileStreamAssemblers[tcpSession->GetFlow()->FiveTuple()];
                    if (assembler && !assembler->IsActive()) {
                        assembler->SetFilename("meterpreter.dll");
                        assembler->TryActivate();
                    }
                }
            }
        }
    }
    return bytesHandled;
}

void MeterpreterPacketHandler::Reset() {
    fileStreamAssemblers.clear();
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
