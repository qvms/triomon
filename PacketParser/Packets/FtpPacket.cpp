#include "FtpPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

const QSet<QString> FtpPacket::userCommandSet = {
    "ABOR", "ACCT", "ADAT", "ALGS", "ALLO", "APPE", "AUTH", "AUTH+", "AVBL",
    "CCC", "CDUP", "CONF", "CSID", "CWD", "DELE", "DSIZ", "ENC", "EPRT",
    "EPSV", "FEAT", "HELP", "HOST", "LANG", "LIST", "LPRT", "LPSV", "MDTM",
    "MFCT", "MFF", "MFMT", "MIC", "MKD", "MLSD", "MLST", "MODE", "NLST",
    "NOOP", "OPTS", "PASS", "PASV", "PBSZ", "PBSZ+", "PORT", "PROT", "PROT+",
    "PWD", "QUIT", "REIN", "REST", "REST+", "RETR", "RMD", "RMDA", "RNFR",
    "RNTO", "SITE", "SIZE", "SMNT", "SPSV", "STAT", "STOR", "STOU", "STRU",
    "SYST", "THMB", "TYPE", "USER", "XCUP", "XCWD", "XMKD", "XPWD", "XRCP",
    "XRMD", "XRSQ", "XSEM", "XSEN", "-N/A-"
};

bool FtpPacket::ContainsLikelyFtpBanner(const QString& line) {
    QString lower = line.toLower();
    return lower.contains("ftp") || lower.contains("filezilla");
}

bool FtpPacket::IsLikelyFtpStart(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, uint16_t sourcePort) {
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (packetStartIndex + 3 > static_cast<int>(dataLen)) return false;

    if (data[packetStartIndex] == 0x32 && data[packetStartIndex + 1] == 0x32 && data[packetStartIndex + 2] == 0x30) {
        if (sourcePort == 21) return true;
        
        int index = packetStartIndex + 3;
        QString line = Utils::ByteConverter::ReadLine(data, index, packetEndIndex - index);
        if (line.toLower().contains("ftp")) return true;
    }
    return false;
}

bool FtpPacket::TryReadToResponseCode(int expectedResponseCode, const uint8_t* data, size_t dataLength, int& index, std::vector<QString>& lines) {
    int responseCodeLastLine = 0;
    QString line;
    do {
        if (index >= static_cast<int>(dataLength)) return false;
        
        line = Utils::ByteConverter::ReadLine(data, index);
        if (line.isNull()) return false;
        
        if (line.length() >= 3) {
            bool ok;
            responseCodeLastLine = line.left(3).toInt(&ok);
            if (!ok) responseCodeLastLine = 0;
        }
    } while (responseCodeLastLine != expectedResponseCode || line.length() < 4 || line.at(3) != ' ');
    return true;
}

bool FtpPacket::TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, uint16_t sourcePort, AbstractPacket*& ftpPacket) {
    ftpPacket = nullptr;
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    try {
        if (clientToServer) {
            if (packetStartIndex >= static_cast<int>(dataLen)) return false;
            char firstChar = static_cast<char>(data[packetStartIndex]);
            if (!QChar(firstChar).isLetter()) return false;

            int index = packetStartIndex;
            QString commandLine = Utils::ByteConverter::ReadLine(data, index);
            if (commandLine.isEmpty()) return false;

            QString command = commandLine;
            int spaceIdx = command.indexOf(' ');
            if (spaceIdx != -1) {
                command = command.left(spaceIdx);
            }
            if (!userCommandSet.contains(command.toUpper())) return false;
        } else {
            if (packetStartIndex + 3 > static_cast<int>(dataLen)) return false;
            if (!QChar(static_cast<char>(data[packetStartIndex])).isDigit()) return false;
            if (!QChar(static_cast<char>(data[packetStartIndex + 1])).isDigit()) return false;
            if (!QChar(static_cast<char>(data[packetStartIndex + 2])).isDigit()) return false;

            int index = packetStartIndex;
            bool ftpConfirmed = false;
            QString line = Utils::ByteConverter::ReadLine(data, index);

            if (!line.isEmpty() && line.length() >= 3) {
                bool ok;
                int responseCode = line.left(3).toInt(&ok);
                if (ok) {
                    if (ContainsLikelyFtpBanner(line)) {
                        ftpConfirmed = true;
                    } else if (line.toLower().contains("smtp")) {
                        return false;
                    }

                    if (line.length() >= 4 && line.at(3) == '-') {
                        std::vector<QString> lines;
                        if (TryReadToResponseCode(responseCode, data, dataLen, index, lines)) {
                            if (!ftpConfirmed && responseCode == 220) {
                                for (const QString& line2 : lines) {
                                    if (line2.toLower().contains("ftp")) {
                                        ftpConfirmed = true;
                                        break;
                                    }
                                }
                                if (!ftpConfirmed && (sourcePort == 25 || sourcePort == 587)) {
                                    return false;
                                }
                            }
                        } else {
                            return false;
                        }
                    }
                }
            }
        }
        ftpPacket = new FtpPacket(parentFrame, packetStartIndex, packetEndIndex, clientToServer);
        return true;
    } catch (...) {
        return false;
    }
}

FtpPacket::FtpPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer)
    : AbstractPacket(parentFrame, packetStartIndex, packetEndIndex, "FTP")
    , clientToServer(clientToServer)
    , responseCode(0)
{
    const uint8_t* data = parentFrame->Data();
    size_t dataLen = parentFrame->DataLength();

    if (clientToServer) {
        int index = PacketStartIndex();
        while (index <= packetEndIndex && index < PacketStartIndex() + 2000 && index < static_cast<int>(dataLen)) {
            QString line = Utils::ByteConverter::ReadLine(data, index);
            int spaceIdx = line.indexOf(' ');
            if (spaceIdx != -1) {
                requestCommand = line.left(spaceIdx);
                if (line.length() > spaceIdx + 1) {
                    requestArgument = line.mid(spaceIdx + 1);
                } else {
                    requestArgument = "";
                }
            } else if (line.length() == 3 || line.length() == 4) {
                requestCommand = line.trimmed();
                requestArgument = "";
            }
        }
    } else {
        int index = PacketStartIndex();
        QString line = Utils::ByteConverter::ReadLine(data, index);
        if (!line.isEmpty()) {
            bool ok;
            responseCode = line.left(3).toInt(&ok);
            if (!ok) responseCode = 0;
            else if (line.length() > 4) {
                responseArgument = line.mid(4);
            } else {
                responseArgument = "";
            }

            if (responseCode > 0 && line.length() > 3) {
                if (line.at(3) == '-') {
                    std::vector<QString> lines;
                    TryReadToResponseCode(responseCode, data, dataLen, index, lines);
                }
            }
        }
        setPacketEndIndex(index - 1);
    }
}

std::vector<AbstractPacket*> FtpPacket::GetSubPackets(bool includeSelfReference) {
    std::vector<AbstractPacket*> subPackets;
    if (includeSelfReference) subPackets.push_back(this);
    return subPackets;
}

} // namespace Packets
} // namespace PacketParser
