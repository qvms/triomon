#include "CaseFile.hpp"
#include <QFileInfo>

namespace NetworkMiner {

CaseFile::CaseFile(const std::string& filePathAndName) 
    : filePathAndName(filePathAndName), framesCount(0), size(0) {
    
    QFileInfo fi(QString::fromStdString(filePathAndName));
    filename = fi.fileName().toStdString();
    
    if (fi.exists()) {
        size = fi.size();
    }
}

void CaseFile::AddMetadata(const std::vector<std::pair<std::string, std::string>>& newMetadata) {
    metadata.insert(metadata.end(), newMetadata.begin(), newMetadata.end());
}

} // namespace NetworkMiner
