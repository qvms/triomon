#include "FileStreamAssemblerList.hpp"

namespace PacketHandlerFramework {
namespace FileTransfer {

FileStreamAssemblerList::FileStreamAssemblerList(std::shared_ptr<MainPacketHandler> packetHandler, int maxPoolSize, const QString& fileOutputDirectory)
    : packetHandler(packetHandler), fileOutputDirectory(fileOutputDirectory), decompressGzipStreams(true) {
}

QString FileStreamAssemblerList::GetAssemblerId(std::shared_ptr<FileStreamAssembler> assembler) {
    return "mocked_id"; // Stub
}

QString FileStreamAssemblerList::GetAssemblerId(void* fiveTuple, bool transferIsClientToServer, const QString& extendedFileId) {
    return "mocked_id"; // Stub
}

void FileStreamAssemblerList::RemoveTempFiles() {
    // Stub
}

void FileStreamAssemblerList::Add(std::shared_ptr<FileStreamAssembler> assembler) {
    // Stub
}

void FileStreamAssemblerList::AddOrEnqueue(std::shared_ptr<FileStreamAssembler> assembler) {
    // Stub
}

void FileStreamAssemblerList::Remove(std::shared_ptr<FileStreamAssembler> assembler, bool closeAssembler) {
    // Stub
}

void FileStreamAssemblerList::Clear(bool removeExtractedFilesFromDisk) {
    // Stub
}

} // namespace FileTransfer
} // namespace PacketHandlerFramework
