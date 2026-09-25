#pragma once

namespace PacketHandlerFramework {
namespace FileTransfer {

enum class FileStreamTypes {
    BackConnect,
    FTP,
    HttpGetChunked,
    HttpGetNormal,
    HttpPost,
    HttpPostMimeMultipartFormData,
    HttpPostMimeFileData,
    HttpPostUpload,
    HTTP2,
    IEC104,
    IMAP,
    LPD,
    MC_NMF,
    Meterpreter,
    njRAT,
    OscarFileTransfer,
    POP3,
    Remcos,
    RTP,
    SMB,
    SMB2,
    SMTP,
    TFTP,
    TlsCertificate,
    VNC
};

} // namespace FileTransfer
} // namespace PacketHandlerFramework
