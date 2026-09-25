#pragma once
#include "AbstractPacket.hpp"
#include "ISessionPacket.hpp"
#include "../Utils.hpp"

namespace PacketParser {
namespace Packets {

class RemcosPacket : public AbstractPacket, public virtual ISessionPacket {
public:
    static const std::vector<uint8_t> REMCOS_MAGIC;
    static const std::vector<uint8_t> DELIMITER;

    enum class RemcosCommand : uint32_t {
        HeartBeat = 0x01,
        InstalledPrograms = 0x03,
        SendNamedFile = 0x05,
        ProcessManager = 0x06,
        WindowManager = 0x08,
        ExecuteCommand = 0x0D,
        CommandLine = 0x0E,
        OpenWebpage = 0x0F,
        ScreenCapture = 0x10,
        GeoIP = 0x11,
        Keylogger = 0x13,
        ClearBrowser = 0x18,
        Webcam = 0x1B,
        Microphone = 0x1D,
        Close = 0x21,
        Uninstall = 0x22,
        Restart = 0x23,
        Update = 0x24,
        MessageBox = 0x26,
        PowerManager = 0x27,
        LoadDLL = 0x2C,
        RegistryEditor = 0x2F,
        ClipboardManager = 0x28,
        RemoteScripting = 0x2E,
        Chat = 0x30,
        Proxy = 0x32,
        ServiceManager = 0x34,
        SendAndExecute = 0x44,
        SystemInfoFull = 0x4b,
        SystemInfoUpdate = 0x4c,
        ScreenCaptureData = 0x4d,
        DownloadRequest = 0x68,
        SetWallpaper = 0x92,
        HostnameAndUser = 0x95,
        ActiveWindowUpdate = 0x96,
        FileManager = 0x98,
        PlaySound = 0xA3,
        DownloadStart = 0xB2,
        DownloadComplete = 0xB3,
        FileSearch = 0x8F,
        UNDEFINED = 0xffffffff,
    };

    uint32_t RemcosPayloadLength;
    uint32_t CommandNumber;

    RemcosPacket(const Frame* parentFrame, int packetStartIndex, int packetEndIndex);

    static bool TryParse(const Frame* parentFrame, int packetStartIndex, int packetEndIndex, AbstractPacket** result);

    bool PacketHeaderIsComplete() const override;
    int ParsedBytesCount() const override;

    bool TryGetCommand(RemcosCommand& command) const;
    std::vector<std::vector<uint8_t>> GetFields(bool skipEmpty, bool requireCompletePacketHeader = true) const;

    std::vector<AbstractPacket*> GetSubPackets(bool includeSelfReference) override;
};

} // namespace Packets
} // namespace PacketParser
