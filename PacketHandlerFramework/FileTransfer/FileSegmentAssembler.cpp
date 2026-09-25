#include "FileSegmentAssembler.hpp"

namespace PacketHandlerFramework {
namespace FileTransfer {

FileSegmentAssembler::FileSegmentAssembler(const std::string& fileOutputDirectory, void* networkTcpSession, bool transferIsClientToServer, const std::string& filePath, const std::string& uniqueFileId, void* fileStreamAssemblerList, void* parentAssemblerList, FileStreamTypes fileStreamType, const std::string& details, const std::string& serverHostname)
    : transferIsClientToServer(transferIsClientToServer), FilePath(filePath), uniqueFileId(uniqueFileId), parentAssemblerList(parentAssemblerList), fileStreamAssemblerList(fileStreamAssemblerList), fileStreamType(fileStreamType), details(details), serverHostname(serverHostname), FileOutputDirectory(fileOutputDirectory) {
}

long long FileSegmentAssembler::TotalFileSize() const {
    if (!IsPartialFile()) {
        return SegmentSize;
    } else if (totalFileNameAndRange.has_value() && totalFileNameAndRange->range.Total > 0) {
        return totalFileNameAndRange->range.Total;
    } else {
        return -1;
    }
}

bool FileSegmentAssembler::IsPartialFile() const {
    return totalFileNameAndRange.has_value() && totalFileNameAndRange->range.Total > SegmentSize;
}

void FileSegmentAssembler::AddData(const std::vector<uint8_t>& fileData, void* frame) {
    // stub
}

} // namespace FileTransfer
} // namespace PacketHandlerFramework
