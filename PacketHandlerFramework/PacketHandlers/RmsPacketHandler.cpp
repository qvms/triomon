#include "RmsPacketHandler.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

RmsPacketHandler::RmsPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> RmsPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::RmsPacket>() };
}

ApplicationLayerProtocol RmsPacketHandler::HandledProtocol() const {
    return ApplicationLayerProtocol::RemoteManipulatorSystem;
}

int RmsPacketHandler::ExtractData(PacketParser::NetworkTcpSession* tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    int bytesHandled = 0;
    
    for (const auto& p : packetList) {
        if (auto rmsPacket = std::dynamic_pointer_cast<PacketParser::Packets::RmsPacket>(p)) {
            bytesHandled += rmsPacket->ParsedBytesCount();
            
            if (rmsPacket->PayloadLength() > 0) {
                QString filename;
                if (transferIsClientToServer) {
                    filename = "rms_client.data";
                } else {
                    filename = "rms_server.data";
                }
                
                auto assembler = std::make_shared<NetworkMiner::FileStreamAssembler>(
                    MainPacketHandler()->FileStreamAssemblerList(),
                    tcpSession->GetFlow()->FiveTuple(),
                    transferIsClientToServer,
                    NetworkMiner::FileStreamTypes::RemoteManipulatorSystem,
                    filename,
                    QString(),
                    QString(),
                    rmsPacket->ParentFrame()->GetFrameNumber(),
                    rmsPacket->ParentFrame()->GetTimestamp()
                );
                
                assembler->SetFileContentLength(rmsPacket->PayloadLength());
                assembler->SetFileSegmentRemainingBytes(rmsPacket->PayloadLength());
                MainPacketHandler()->FileStreamAssemblerList()->Add(assembler);
            }
        }
    }
    
    return bytesHandled;
}

void RmsPacketHandler::Reset() {
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
