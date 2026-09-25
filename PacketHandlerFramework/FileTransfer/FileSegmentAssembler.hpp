#pragma once

#include <string>
#include <map>
#include <QDateTime>
#include "PacketHandlerFramework/FileTransfer/FileStreamTypes.hpp"
#include "PacketParser/FileTransfer/ContentRange.hpp"
#include <optional>

namespace PacketHandlerFramework {
namespace FileTransfer {

class FileSegmentAssembler {
private:
    void* fiveTuple; // mock
    bool transferIsClientToServer;
    void* fileStream; // mock
    std::string tempFilePath;
    std::string uniqueFileId;
    void* parentAssemblerList; // mock
    void* fileStreamAssemblerList; // mock
    FileStreamTypes fileStreamType;
    std::string details;
    long long initialFrameNumber = -1;
    QDateTime initialTimeStamp;
    std::string serverHostname;
    std::map<long long, int> segmentOffsetBytesWritten;

public:
    std::string FilePath;
    std::string ContentEncoding;
    std::string ContentType;
    long long SegmentSize = -1;
    
    struct TotalFileNameAndRange {
        std::string key;
        PacketParser::FileTransfer::ContentRange range;
    };
    std::optional<TotalFileNameAndRange> totalFileNameAndRange;
    std::string FileOutputDirectory;

    FileSegmentAssembler(const std::string& fileOutputDirectory, void* networkTcpSession, bool transferIsClientToServer, const std::string& filePath, const std::string& uniqueFileId, void* fileStreamAssemblerList, void* parentAssemblerList, FileStreamTypes fileStreamType, const std::string& details, const std::string& serverHostname);

    bool IsEmpty() const { return segmentOffsetBytesWritten.empty(); }
    long long TotalFileSize() const;
    bool IsPartialFile() const;

    void AddData(const std::vector<uint8_t>& fileData, void* frame);
    // void AddData(long long offset, const std::vector<uint8_t>& fileData, void* frame);
};

} // namespace FileTransfer
} // namespace PacketHandlerFramework
