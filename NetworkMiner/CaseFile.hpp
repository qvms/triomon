#pragma once

#include <string>
#include <vector>
#include <utility>

namespace NetworkMiner {

class CaseFile {
private:
    std::string filename;
    std::string filePathAndName;
    std::string md5;
    std::string firstFrameTimestamp;
    std::string lastFrameTimestamp;
    int framesCount;
    std::vector<std::pair<std::string, std::string>> metadata;
    long long size;
    std::string parsingTime; // Simplified timespan equivalent

public:
    explicit CaseFile(const std::string& filePathAndName);

    std::string get_Filename() const { return filename; }
    std::string get_FilePathAndName() const { return filePathAndName; }

    std::string get_Md5() const { return md5; }
    void set_Md5(const std::string& value) { md5 = value; }

    std::string get_FirstFrameTimestamp() const { return firstFrameTimestamp; }
    void set_FirstFrameTimestamp(const std::string& value) { firstFrameTimestamp = value; }

    std::string get_LastFrameTimestamp() const { return lastFrameTimestamp; }
    void set_LastFrameTimestamp(const std::string& value) { lastFrameTimestamp = value; }

    int get_FramesCount() const { return framesCount; }
    void set_FramesCount(int value) { framesCount = value; }

    const std::vector<std::pair<std::string, std::string>>& get_Metadata() const { return metadata; }
    long long get_Size() const { return size; }

    std::string get_ParsingTime() const { return parsingTime; }
    void set_ParsingTime(const std::string& value) { parsingTime = value; }

    void AddMetadata(const std::vector<std::pair<std::string, std::string>>& newMetadata);
};

} // namespace NetworkMiner
