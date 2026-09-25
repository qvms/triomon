#pragma once

namespace PacketHandlerFramework {
    namespace FileTransfer {
        class ReconstructedFile;
    }
}

namespace PacketParser {
namespace Events {

class FileEventArgs {
public:
    PacketHandlerFramework::FileTransfer::ReconstructedFile* File;
    FileEventArgs(PacketHandlerFramework::FileTransfer::ReconstructedFile* file) : File(file) {}
};

}
}