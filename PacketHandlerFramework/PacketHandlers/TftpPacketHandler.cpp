#include "TftpPacketHandler.hpp"
#include "../../SharedUtils/Logger.hpp"

namespace PacketHandlerFramework {
namespace PacketHandlers {

TftpPacketHandler::TftpPacketHandler(PacketHandler* mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

QList<int> TftpPacketHandler::ParsedTypes() const {
    return { qMetaTypeId<PacketParser::Packets::TftpPacket>() };
}

void TftpPacketHandler::ExtractData(NetworkHost* sourceHost, NetworkHost* destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::Packets::UdpPacket> udpPacket;
    
    for (const auto& p : packetList) {
        if (auto udp = std::dynamic_pointer_cast<PacketParser::Packets::UdpPacket>(p)) {
            udpPacket = udp;
        } else if (udpPacket) {
            std::shared_ptr<NetworkMiner::FileStreamAssembler> assembler;
            bool assemblerIsFound = TryGetTftpFileStreamAssembler(assembler, MainPacketHandler()->FileStreamAssemblerList(), sourceHost, udpPacket->SourcePort(), destinationHost, udpPacket->DestinationPort());
            
            std::shared_ptr<PacketParser::Packets::TftpPacket> tftpPacket;
            if (assemblerIsFound) {
                QString sessionId = GetTftpSessionId(sourceHost, udpPacket->SourcePort(), destinationHost, udpPacket->DestinationPort());
                int blksize = PacketParser::Packets::TftpPacket::DEFAULT_BLKSIZE;
                if (tftpSessionBlksizeList.contains(sessionId)) {
                    blksize = tftpSessionBlksizeList[sessionId];
                }
                
                try {
                    PacketParser::Packets::TftpPacket::TryParse(p->ParentFrame(), p->PacketStartIndex(), p->PacketEndIndex(), blksize, p);
                    tftpPacket = std::dynamic_pointer_cast<PacketParser::Packets::TftpPacket>(p);
                } catch (const std::exception& e) {
                    SharedUtils::Logger::Log("Error parsing TFTP packet: " + QString(e.what()), SharedUtils::Logger::EventLogEntryType::Warning);
                }
            }
            
            if (!assemblerIsFound || tftpPacket == nullptr) {
                try {
                    PacketParser::Packets::TftpPacket::TryParse(p->ParentFrame(), p->PacketStartIndex(), p->PacketEndIndex(), p);
                    tftpPacket = std::dynamic_pointer_cast<PacketParser::Packets::TftpPacket>(p);
                    
                    if (tftpPacket) {
                        if (TryCreateNewAssembler(assembler, MainPacketHandler()->FileStreamAssemblerList(), tftpPacket, sourceHost, udpPacket->SourcePort(), destinationHost)) {
                            assemblerIsFound = true;
                            if (tftpPacket->Blksize() != PacketParser::Packets::TftpPacket::DEFAULT_BLKSIZE) {
                                QString sessionId = GetTftpSessionId(sourceHost, udpPacket->SourcePort(), destinationHost, udpPacket->DestinationPort());
                                tftpSessionBlksizeList.insert(sessionId, tftpPacket->Blksize());
                            }
                        }
                    }
                } catch (const std::exception& e) {
                    if (assembler) {
                        SharedUtils::Logger::Log("Error parsing TFTP packet: " + QString(e.what()), SharedUtils::Logger::EventLogEntryType::Warning);
                    }
                }
            }
            
            if (tftpPacket) {
                ExtractFileData(assembler, MainPacketHandler()->FileStreamAssemblerList(), sourceHost, udpPacket->SourcePort(), destinationHost, udpPacket->DestinationPort(), tftpPacket);
            }
        }
    }
}

void TftpPacketHandler::Reset() {
    tftpSessionBlksizeList.clear();
}

bool TftpPacketHandler::TryGetTftpFileStreamAssembler(std::shared_ptr<NetworkMiner::FileStreamAssembler>& assembler, PacketParser::FileStreamAssemblerList* fileStreamAssemblerList, NetworkHost* sourceHost, quint16 sourcePort, NetworkHost* destinationHost, quint16 destinationPort) {
    PacketParser::FiveTuple tmpFiveTuple(sourceHost, sourcePort, destinationHost, destinationPort, PacketParser::FiveTuple::TransportProtocol::UDP);
    if (fileStreamAssemblerList->ContainsAssembler(tmpFiveTuple, true)) {
        assembler = fileStreamAssemblerList->GetAssembler(tmpFiveTuple, true);
        if (assembler->FileStreamType() == NetworkMiner::FileStreamTypes::TFTP) return true;
        else assembler = nullptr;
    }
    
    tmpFiveTuple = PacketParser::FiveTuple(sourceHost, PacketParser::Packets::TftpPacket::DefaultUdpPortNumber, destinationHost, destinationPort, PacketParser::FiveTuple::TransportProtocol::UDP);
    if (fileStreamAssemblerList->ContainsAssembler(tmpFiveTuple, true)) {
        assembler = fileStreamAssemblerList->GetAssembler(tmpFiveTuple, true);
        if (assembler->FileStreamType() == NetworkMiner::FileStreamTypes::TFTP) return true;
        else assembler = nullptr;
    }
    
    tmpFiveTuple = PacketParser::FiveTuple(sourceHost, sourcePort, destinationHost, PacketParser::Packets::TftpPacket::DefaultUdpPortNumber, PacketParser::FiveTuple::TransportProtocol::UDP);
    if (fileStreamAssemblerList->ContainsAssembler(tmpFiveTuple, true)) {
        assembler = fileStreamAssemblerList->GetAssembler(tmpFiveTuple, true);
        if (assembler->FileStreamType() == NetworkMiner::FileStreamTypes::TFTP) return true;
        else assembler = nullptr;
    }
    
    assembler = nullptr;
    return false;
}

bool TftpPacketHandler::TryCreateNewAssembler(std::shared_ptr<NetworkMiner::FileStreamAssembler>& assembler, PacketParser::FileStreamAssemblerList* fileStreamAssemblerList, std::shared_ptr<PacketParser::Packets::TftpPacket> tftpPacket, NetworkHost* sourceHost, quint16 sourcePort, NetworkHost* destinationHost) {
    assembler = nullptr;
    
    if (tftpPacket->OpCode() == PacketParser::Packets::TftpPacket::OpCodes::ReadRequest || tftpPacket->OpCode() == PacketParser::Packets::TftpPacket::OpCodes::WriteRequest) {
        try {
            PacketParser::FiveTuple tmpFiveTuple;
            if (tftpPacket->OpCode() == PacketParser::Packets::TftpPacket::OpCodes::ReadRequest) {
                tmpFiveTuple = PacketParser::FiveTuple(destinationHost, PacketParser::Packets::TftpPacket::DefaultUdpPortNumber, sourceHost, sourcePort, PacketParser::FiveTuple::TransportProtocol::UDP);
            } else {
                tmpFiveTuple = PacketParser::FiveTuple(sourceHost, sourcePort, destinationHost, PacketParser::Packets::TftpPacket::DefaultUdpPortNumber, PacketParser::FiveTuple::TransportProtocol::UDP);
            }
            
            assembler = std::make_shared<NetworkMiner::FileStreamAssembler>(
                fileStreamAssemblerList, tmpFiveTuple, true, NetworkMiner::FileStreamTypes::TFTP,
                tftpPacket->Filename(), "", QString::number(static_cast<int>(tftpPacket->OpCode())) + " " + QString::number(static_cast<int>(tftpPacket->Mode())) + " " + tftpPacket->Filename(),
                tftpPacket->ParentFrame()->GetFrameNumber(), tftpPacket->ParentFrame()->GetTimestamp()
            );
            fileStreamAssemblerList->Add(assembler);
            return true;
        } catch (const std::exception& e) {
            SharedUtils::Logger::Log("Error creating assembler for TFTP file transfer in frame. " + QString(e.what()), SharedUtils::Logger::EventLogEntryType::Information);
            if (assembler) {
                assembler->Clear();
                assembler = nullptr;
            }
            return false;
        }
    }
    
    return false;
}

QString TftpPacketHandler::GetTftpSessionId(NetworkHost* sourceHost, quint16 sourcePort, NetworkHost* destinationHost, quint16 destinationPort) {
    QString sourceString = sourceHost->IPAddress().toString() + "\t" + QString::number(sourcePort);
    QString destinationString = destinationHost->IPAddress().toString() + "\t" + QString::number(destinationPort);
    if (sourceString > destinationString) {
        return sourceString + "\t" + destinationString;
    } else {
        return destinationString + "\t" + sourceString;
    }
}

void TftpPacketHandler::ExtractFileData(std::shared_ptr<NetworkMiner::FileStreamAssembler> assembler, PacketParser::FileStreamAssemblerList* fileStreamAssemblerList, NetworkHost* sourceHost, quint16 sourcePort, NetworkHost* destinationHost, quint16 destinationPort, std::shared_ptr<PacketParser::Packets::TftpPacket> tftpPacket) {
    if (tftpPacket->OpCode() == PacketParser::Packets::TftpPacket::OpCodes::Data && assembler) {
        if (!assembler->IsActive()) {
            if (assembler->SourcePort() != sourcePort || assembler->DestinationPort() != destinationPort) {
                fileStreamAssemblerList->Remove(assembler, true);
                PacketParser::FiveTuple tmpFiveTuple(sourceHost, sourcePort, destinationHost, destinationPort, PacketParser::FiveTuple::TransportProtocol::UDP);
                assembler = std::make_shared<NetworkMiner::FileStreamAssembler>(
                    fileStreamAssemblerList, tmpFiveTuple, true, NetworkMiner::FileStreamTypes::TFTP,
                    assembler->Filename(), assembler->FileLocation(), assembler->Details(),
                    tftpPacket->ParentFrame()->GetFrameNumber(), tftpPacket->ParentFrame()->GetTimestamp()
                );
                fileStreamAssemblerList->Add(assembler);
            }
            assembler->TryActivate();
        }
        
        if (assembler->SourceHost() == sourceHost && assembler->SourcePort() == sourcePort && assembler->DestinationHost() == destinationHost && assembler->DestinationPort() == destinationPort) {
            assembler->AddData(tftpPacket->DataBlock(), tftpPacket->DataBlockNumber());
            if (tftpPacket->DataBlockIsLast()) {
                assembler->FinishAssembling();
            }
        }
    }
}

} // namespace PacketHandlers
} // namespace PacketHandlerFramework
