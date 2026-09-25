#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"
#include <QStringList>
#include <vector>

namespace PacketParser {
namespace Packets {

class LpdPacket : public AbstractPacket, public virtual ISessionPacket {
protected:
    LpdPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

public:
    virtual ~LpdPacket() = default;

    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, AbstractPacket** result);

    bool PacketHeaderIsComplete() const override;
    virtual int ParsedBytesCount() const override = 0;

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;

    }; // End LpdPacket

class LpdRequestPacket : public LpdPacket {
    public:
        enum class DaemonCommandCode : uint8_t {
            PrintWaitingJobs = 1,
            ReceiveJob = 2,
            SendQueueStateShort = 3,
            SendQueueStateLong = 4,
            RemoveJobs = 5
        };

        enum class ReceiveJobSubcommandCode : uint8_t {
            AbortJob = 1,
            ReceiveControlFile = 2,
            ReceiveDataFile = 3
        };

        static bool TryParseCommandLine(const uint8_t* data, int startIndex, int endIndex, uint8_t& commandCode, QStringList& operands, int& bytesRead);

        uint8_t CommandCode;
        QStringList Operands;

        LpdRequestPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

        int ParsedBytesCount() const override;

        bool TryGetCommand(DaemonCommandCode& command) const;
        bool TryGetSubCommand(ReceiveJobSubcommandCode& subCommand) const;

    private:
        int m_parsedBytesCount;
    };

class LpdResponsePacket : public LpdPacket {
    public:
        uint8_t ResponseCode;

        LpdResponsePacket(const Frame* parentFrame, int packetStartIndex);

        int ParsedBytesCount() const override;
    };

class LpdControlFilePacket : public LpdPacket {
    public:
        enum class Command : uint8_t {
            Class = 'C',
            Host = 'H',
            Indent = 'I',
            JobName = 'J',
            PrintBanner = 'L',
            Mail = 'M',
            NameOfSourceFile = 'N',
            UserID = 'P',
            SymbolicLink = 'S',
            Title = 'T',
            Unlink = 'U',
            Width = 'W',
            FontFileR = '1',
            FontFileI = '2',
            FontFileB = '3',
            FontFileS = '4',
            PlotCifFile = 'c',
            PrintDviFile = 'd',
            PrintFormattedFile = 'f',
            PlotFile = 'g',
            Kerberos = 'k',
            LeaveControlChars = 'l',
            PrintDitroffFile = 'n',
            PrintPostscriptFile = 'o',
            PrintPrFormat = 'p',
            PrintFortran = 'r',
            PrintTroffFile = 't',
            PrintRasterFile = 'v'
        };

        static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result);

        std::vector<std::pair<Command, QString>> CommandList;

        LpdControlFilePacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

        int ParsedBytesCount() const override;
    };
};

} // namespace Packets
