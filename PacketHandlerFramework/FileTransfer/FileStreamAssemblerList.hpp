#pragma once

#include "PacketHandlerFramework/FileTransfer/FileStreamAssembler.hpp"
#include <map>
#include <memory>
#include <vector>
#include <queue>
#include <QString>
#include <mutex>
#include "PacketHandlerFramework/FiveTuple.hpp" // Placeholder

namespace PacketHandlerFramework {

class MainPacketHandler; // Forward declaration

namespace FileTransfer {

class FileStreamAssemblerList {
private:
    std::shared_ptr<MainPacketHandler> packetHandler;
    QString fileOutputDirectory;
    bool decompressGzipStreams;
    
    std::map<QString, std::shared_ptr<FileStreamAssembler>> assemblers;
    std::map<QString, std::queue<std::shared_ptr<FileStreamAssembler>>> fileStreamAssemblerQueue;
    
    QString GetAssemblerId(std::shared_ptr<FileStreamAssembler> assembler);
    QString GetAssemblerId(void* fiveTuple, bool transferIsClientToServer, const QString& extendedFileId = ""); // fiveTuple is void* mock for now

public:
    FileStreamAssemblerList(std::shared_ptr<MainPacketHandler> packetHandler, int maxPoolSize, const QString& fileOutputDirectory);
    ~FileStreamAssemblerList() = default;
    
    static void RemoveTempFiles();
    
    void Add(std::shared_ptr<FileStreamAssembler> assembler);
    void AddOrEnqueue(std::shared_ptr<FileStreamAssembler> assembler);
    void Remove(std::shared_ptr<FileStreamAssembler> assembler, bool closeAssembler);
    
    void Clear(bool removeExtractedFilesFromDisk = false);
    bool ContainsAssembler(std::shared_ptr<FileStreamAssembler> assembler);
    bool ContainsAssembler(void* clientHost, uint16_t clientPort, void* serverHost, uint16_t serverPort, bool tcpTransfer);
    std::shared_ptr<FileStreamAssembler> GetAssembler(void* clientHost, uint16_t clientPort, void* serverHost, uint16_t serverPort, bool tcpTransfer);
};

} // namespace FileTransfer
} // namespace PacketHandlerFramework
