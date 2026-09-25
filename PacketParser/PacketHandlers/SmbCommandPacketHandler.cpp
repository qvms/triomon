#include "SmbCommandPacketHandler.hpp"
#include <QStringList>

namespace PacketParser {
namespace PacketHandlers {

SmbCommandPacketHandler::SmbCommandPacketHandler(std::shared_ptr<PacketHandlerFramework::MainPacketHandler> mainPacketHandler)
    : PacketHandlerFramework::PacketHandlers::AbstractPacketHandler(mainPacketHandler), smbSessionPopularityList(100) {
}

bool SmbCommandPacketHandler::TryExtractData(std::shared_ptr<NetworkTcpSession> tcpSession, std::shared_ptr<NetworkHost> sourceHost, std::shared_ptr<NetworkHost> destinationHost, const QList<Packets::AbstractPacket*>& packetList) {
    bool successfulExtraction = false;

    Packets::AbstractSmbCommand* smbCommandPacket = nullptr;
    Packets::TcpPacket* tcpPacket = nullptr;
    
    for (Packets::AbstractPacket* p : packetList) {
        if (auto* smb = dynamic_cast<Packets::AbstractSmbCommand*>(p)) {
            smbCommandPacket = smb;
        } else if (auto* tcp = dynamic_cast<Packets::TcpPacket*>(p)) {
            tcpPacket = tcp;
        }
    }

    if (tcpPacket != nullptr && smbCommandPacket != nullptr) {
        successfulExtraction = true;
        ExtractSmbData(sourceHost, destinationHost, tcpPacket, smbCommandPacket, GetMainPacketHandler());
    }
    return successfulExtraction;
}

void SmbCommandPacketHandler::ExtractSmbData(std::shared_ptr<NetworkHost> sourceHost, std::shared_ptr<NetworkHost> destinationHost, Packets::TcpPacket* tcpPacket, Packets::AbstractSmbCommand* smbCommandPacket, std::shared_ptr<PacketHandlerFramework::MainPacketHandler> mainPacketHandler) {
    QString smbSessionId;
    if (smbCommandPacket->FlagsResponse) {
        smbSessionId = SmbSession::GetSmbSessionId(sourceHost->IPAddress(), tcpPacket->SourcePort(), destinationHost->IPAddress(), tcpPacket->DestinationPort());
    } else {
        smbSessionId = SmbSession::GetSmbSessionId(destinationHost->IPAddress(), tcpPacket->DestinationPort(), sourceHost->IPAddress(), tcpPacket->SourcePort());
    }

    if (auto* req = dynamic_cast<Packets::NegotiateProtocolRequest*>(smbCommandPacket)) {
        // sourceHost.AcceptedSmbDialectsList = request.DialectList; // Stubbed list assignment
    } else if (auto* reply = dynamic_cast<Packets::NegotiateProtocolResponse*>(smbCommandPacket)) {
        // ... update preferred dialect 
    } else if (auto* reqSetup = dynamic_cast<Packets::SetupAndXRequest*>(smbCommandPacket)) {
        // Extra info populate...
        if (!reqSetup->AccountName.isEmpty()) {
            auto nCredential = std::make_shared<PacketHandlerFramework::NetworkCredential>(
                sourceHost, destinationHost, "SetupAndXRequest", reqSetup->AccountName.toStdString(),
                reqSetup->AccountPassword.toStdString(), QDateTime(), reqSetup->PrimaryDomain.toStdString());
            mainPacketHandler->AddCredential(nCredential);
        }
    } else if (auto* resSetup = dynamic_cast<Packets::SetupAndXResponse*>(smbCommandPacket)) {
        // ... more extra info
    } else if (auto* reqCreate = dynamic_cast<Packets::NTCreateAndXRequest*>(smbCommandPacket)) {
        QString filename = reqCreate->Filename;
        if (filename.endsWith('\0')) filename.chop(1);
        
        QString filePath;
        if (filename.contains('\\')) {
            int lastSlash = filename.lastIndexOf('\\');
            filePath = filename.left(lastSlash);
            filename = filename.mid(lastSlash + 1);
        } else {
            filePath = "\\";
        }

        try {
            std::shared_ptr<SmbSession> smbSession;
            if (smbSessionPopularityList.ContainsKey(smbSessionId)) {
                smbSessionPopularityList.TryGetValue(smbSessionId, smbSession);
            } else {
                smbSession = std::make_shared<SmbSession>(destinationHost->IPAddress(), tcpPacket->DestinationPort(), sourceHost->IPAddress(), tcpPacket->SourcePort());
                smbSessionPopularityList.Add(smbSessionId, smbSession);
            }
            
            // Using stub properties to construct FileStreamAssembler
            auto assembler = std::make_shared<PacketHandlerFramework::FileTransfer::FileStreamAssembler>(
                mainPacketHandler->FileStreamAssemblerList.get(), destinationHost->IPAddress(), tcpPacket->DestinationPort(), sourceHost->IPAddress(), tcpPacket->SourcePort(), tcpPacket != nullptr, 
                PacketParser::FileStreamTypes::SMB, filename, filePath, reqCreate->Filename, 0, QDateTime::currentDateTime()); // Frame/Timestamp mocked
                
            smbSession->AddFileStreamAssembler(assembler, smbCommandPacket->ParentCifsPacket->TreeId());
        } catch (std::exception& e) {
            mainPacketHandler->OnAnomalyDetected(QString("Error creating assembler for SMB file transfer: ") + e.what());
        }
    } else if (!smbCommandPacket->FlagsResponse && smbSessionPopularityList.ContainsKey(smbSessionId)) {
        if (auto* reqClose = dynamic_cast<Packets::CloseRequest*>(smbCommandPacket)) {
            std::shared_ptr<SmbSession> smbSession;
            smbSessionPopularityList.TryGetValue(smbSessionId, smbSession);
            
            uint16_t fileId = reqClose->FileId;
            uint16_t treeId = smbCommandPacket->ParentCifsPacket->TreeId();
            if (smbSession->ContainsFileId(treeId, fileId)) {
                auto assemblerToClose = smbSession->GetFileStreamAssembler(treeId, fileId);
                smbSession->RemoveFileStreamAssembler(treeId, fileId, false);
                
                if (mainPacketHandler->FileStreamAssemblerList->ContainsAssembler(assemblerToClose)) {
                    mainPacketHandler->FileStreamAssemblerList->Remove(assemblerToClose, true);
                } else {
                    assemblerToClose->clear();
                }
            }
        } else if (auto* reqRead = dynamic_cast<Packets::ReadAndXRequest*>(smbCommandPacket)) {
            std::shared_ptr<SmbSession> smbSession;
            smbSessionPopularityList.TryGetValue(smbSessionId, smbSession);
            smbSession->Touch(smbCommandPacket->ParentCifsPacket->TreeId(), reqRead->FileId);
        }
    } else if (smbCommandPacket->FlagsResponse && smbSessionPopularityList.ContainsKey(smbSessionId)) {
        std::shared_ptr<SmbSession> smbSession;
        smbSessionPopularityList.TryGetValue(smbSessionId, smbSession);

        if (auto* resCreate = dynamic_cast<Packets::NTCreateAndXResponse*>(smbCommandPacket)) {
            uint16_t treeId = smbCommandPacket->ParentCifsPacket->TreeId();
            auto assembler = smbSession->GetLastReferencedFileStreamAssembler(treeId);
            smbSession->RemoveLastReferencedAssembler(treeId);
            if (assembler) {
                smbSession->AddFileStreamAssembler(assembler, treeId, resCreate->FileId);
                assembler->setFileContentLength(resCreate->EndOfFile);
            }
        } else if (auto* resRead = dynamic_cast<Packets::ReadAndXResponse*>(smbCommandPacket)) {
            uint16_t treeId = smbCommandPacket->ParentCifsPacket->TreeId();
            auto assembler = smbSession->GetLastReferencedFileStreamAssembler(treeId);
            if (assembler) {
                if (!mainPacketHandler->FileStreamAssemblerList->ContainsAssembler(assembler)) {
                    mainPacketHandler->FileStreamAssemblerList->Add(assembler);
                }
                
                assembler->setFileSegmentRemainingBytes(assembler->getFileSegmentRemainingBytes() + resRead->DataLength);
                if (!assembler->getIsActive()) {
                    if (!assembler->tryActivate()) {
                        mainPacketHandler->OnAnomalyDetected("Unable to activate file stream assembler for " + assembler->getFileLocation() + "/" + assembler->getFilename());
                    } else {
                        assembler->addData(resRead->GetFileData(), tcpPacket->SequenceNumber());
                    }
                }
                
                if (!assembler->getIsActive()) {
                    smbSession->RemoveLastReferencedAssembler(treeId);
                }
            }
        }
    }
}

} // namespace PacketHandlers
} // namespace PacketParser
