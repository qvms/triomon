#pragma once

#include "PacketHandlerFramework/PacketHandlers/AbstractPacketHandler.hpp"
#include "PacketParser/PacketHandlers/ITcpSessionPacketHandler.hpp"
#include "PacketParser/NetworkTcpSession.hpp"
#include "PacketParser/NetworkHost.hpp"
#include "PacketParser/Packets/AbstractPacket.hpp"
#include "PacketParser/Packets/CifsPacket.hpp"
#include "PacketParser/Packets/TcpPacket.hpp"
#include "PacketHandlerFramework/MainPacketHandler.hpp"
#include "PacketParser/PopularityList.hpp"
#include "PacketHandlerFramework/FileTransfer/FileStreamAssembler.hpp"
#include "PacketHandlerFramework/NetworkCredential.hpp"
#include <QList>
#include <QHostAddress>
#include <memory>

namespace PacketParser {
namespace PacketHandlers {

class SmbCommandPacketHandler : public PacketHandlerFramework::PacketHandlers::AbstractPacketHandler, public ITcpSessionPacketHandler {
private:
    class SmbSession {
    private:
        QHostAddress serverIP;
        QHostAddress clientIP;
        uint16_t serverTcpPort;
        uint16_t clientTcpPort;
        
        std::map<uint16_t, uint16_t> lastReferencedFileIdPerTreeId;
        PopularityList<uint16_t, std::shared_ptr<PacketHandlerFramework::FileTransfer::FileStreamAssembler>> fileIdAssemblerList;

    public:
        static QString GetSmbSessionId(const QHostAddress& serverIP, uint16_t serverTcpPort, const QHostAddress& clientIP, uint16_t clientTcpPort) {
            return serverIP.toString() + ":" + QString::number(serverTcpPort, 16).rightJustified(4, '0') + "-" +
                   clientIP.toString() + ":" + QString::number(clientTcpPort, 16).rightJustified(4, '0');
        }

        SmbSession(const QHostAddress& serverIP, uint16_t serverTcpPort, const QHostAddress& clientIP, uint16_t clientTcpPort)
            : serverIP(serverIP), serverTcpPort(serverTcpPort), clientIP(clientIP), clientTcpPort(clientTcpPort), fileIdAssemblerList(100) {}

        QString GetId() const {
            return GetSmbSessionId(serverIP, serverTcpPort, clientIP, clientTcpPort);
        }

        bool ContainsFileId(uint16_t treeId, uint16_t fileId) const {
            return fileIdAssemblerList.ContainsKey(fileId);
        }

        void AddFileStreamAssembler(std::shared_ptr<PacketHandlerFramework::FileTransfer::FileStreamAssembler> assembler, uint16_t treeId, uint16_t fileId = 0) {
            lastReferencedFileIdPerTreeId[treeId] = fileId;
            fileIdAssemblerList.Remove(fileId);
            fileIdAssemblerList.Add(fileId, assembler);
        }

        void RemoveLastReferencedAssembler(uint16_t treeId) {
            uint16_t lastReferencedFileId = lastReferencedFileIdPerTreeId.count(treeId) ? lastReferencedFileIdPerTreeId[treeId] : 0;
            if (fileIdAssemblerList.ContainsKey(lastReferencedFileId)) {
                RemoveFileStreamAssembler(treeId, lastReferencedFileId);
            }
        }

        void RemoveFileStreamAssembler(uint16_t treeId, uint16_t fileId, bool closeAssembler = false) {
            std::shared_ptr<PacketHandlerFramework::FileTransfer::FileStreamAssembler> assembler;
            if (fileIdAssemblerList.TryGetValue(fileId, assembler)) {
                fileIdAssemblerList.Remove(fileId);
                if (closeAssembler && assembler) {
                    assembler->clear();
                }
            }
        }

        std::shared_ptr<PacketHandlerFramework::FileTransfer::FileStreamAssembler> GetLastReferencedFileStreamAssembler(uint16_t treeId) {
            if (lastReferencedFileIdPerTreeId.count(treeId)) {
                return GetFileStreamAssembler(treeId, lastReferencedFileIdPerTreeId[treeId]);
            }
            return nullptr;
        }

        std::shared_ptr<PacketHandlerFramework::FileTransfer::FileStreamAssembler> GetFileStreamAssembler(uint16_t treeId, uint16_t fileId) {
            lastReferencedFileIdPerTreeId[treeId] = fileId;
            std::shared_ptr<PacketHandlerFramework::FileTransfer::FileStreamAssembler> assembler;
            if (fileIdAssemblerList.TryGetValue(fileId, assembler)) {
                return assembler;
            }
            return nullptr;
        }

        void Touch(uint16_t treeId, uint16_t fileId) {
            if (fileIdAssemblerList.ContainsKey(fileId)) {
                lastReferencedFileIdPerTreeId[treeId] = fileId;
            }
        }
    };

    PopularityList<QString, std::shared_ptr<SmbSession>> smbSessionPopularityList;

    void ExtractSmbData(std::shared_ptr<NetworkHost> sourceHost, std::shared_ptr<NetworkHost> destinationHost, Packets::TcpPacket* tcpPacket, Packets::AbstractSmbCommand* smbCommandPacket, std::shared_ptr<PacketHandlerFramework::MainPacketHandler> mainPacketHandler);

public:
    SmbCommandPacketHandler(std::shared_ptr<PacketHandlerFramework::MainPacketHandler> mainPacketHandler);

    void ExtractDataFromPacket(void* transportPacket) override {}
    bool TryExtractData(std::shared_ptr<NetworkTcpSession> tcpSession, std::shared_ptr<NetworkHost> sourceHost, std::shared_ptr<NetworkHost> destinationHost, const QList<Packets::AbstractPacket*>& packetList);
};

} // namespace PacketHandlers
} // namespace PacketParser
