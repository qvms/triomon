#pragma once

#include "PacketParser/Packets/AbstractPacket.hpp"
#include "PacketParser/Packets/Frame.hpp"
#include <QColor>
#include <QSize>
#include <QString>
#include <QVector>
#include <QByteArray>
#include <memory>
#include <optional>

namespace PacketParser {
namespace Packets {

class VncCommandPacket;
class VncResponsePacket;
class KeyEventPacket;
class FixColorMapPacket;
class SetPixelFormatPacket;
class SetEncodingsPacket;
class FrameBufferUpdateRequestPacket;
class ClientCutTextPacket;
class Rectangle;
class ServerCutTextPacket;
class FrameBufferUpdatePacket;

class RfbPacket : public AbstractPacket {
public:
    enum class SecurityType : uint8_t {
        Invalid = 0,
        None = 1,
        VncAuth = 2,
        RSA_AES = 5,
        RSA_AES_Unencrypted = 6,
        SSPI = 7,
        SSPIne = 8,
        Tight = 16,
        Ultra = 17,
        TLS = 18,
        VeNCrypt = 19,
        GTK_VNC_SASL = 20,
        MD5 = 21,
        XVP = 22,
        MAC_OSX_SECTYPE_30 = 30,
        MAC_OSX_SECTYPE_35 = 35,
        ARD = 30,
        TIGHT_AUTH_TGHT_ULGNAUTH = 119,
        TIGHT_AUTH_TGHT_XTRNAUTH = 130,
    };

    struct VncPixelFormat {
        uint8_t BitsPerPixel;
        uint8_t Depth;
        bool BigEndian;
        bool TrueColour;
        uint16_t RedMax;
        uint16_t GreenMax;
        uint16_t BlueMax;
        uint8_t RedShift;
        uint8_t GreenShift;
        uint8_t BlueShift;

        VncPixelFormat() = default;
        VncPixelFormat(const QByteArray& data, int offset);
        VncPixelFormat(const VncPixelFormat& original, bool switchRedAndBlue);

        int BytesPerPixel() const { return (BitsPerPixel + 7) / 8; }
        bool TryGetColor(const QByteArray& data, int offset, QColor& color) const;
        bool TryGetColor(uint32_t colorValue, QColor& color) const;
        QString ToString() const;

    private:
        static uint8_t GetColorIntensity(uint32_t colorValue, uint8_t shift, uint16_t max);
    };

    static bool TryParseVncPixelFormat(const QByteArray& data, int offset, VncPixelFormat& vncPixelFormat);
    static bool TryGetCutTextPacketLength(Frame* parentFrame, int packetStartIndex, int packetEndIndex, int& cutTextPacketLength);
    static bool TryParseHandshake(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, std::shared_ptr<RfbPacket>& rfbPacket);
    static bool TryParse(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, std::shared_ptr<RfbPacket>& rfbPacket);

    std::shared_ptr<VncCommandPacket> CommandPacket;
    std::shared_ptr<VncResponsePacket> ResponsePacket;
    QString ProtocolVersionString;
    QVector<SecurityType> SecurityTypes;
    std::optional<VncPixelFormat> PixelFormat;
    std::optional<QSize> ScreenSize;
    QString VncDesktopName;

    explicit RfbPacket(std::shared_ptr<VncCommandPacket> commandPacket);
    explicit RfbPacket(std::shared_ptr<VncResponsePacket> responsePacket);
    RfbPacket(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool transferIsClientToServer);

    QVector<std::shared_ptr<AbstractPacket>> GetSubPackets(bool includeSelfReference) override;

protected:
    static const int MAX_SCREEN_WIDTH = 8000;
    static const int MAX_SCREEN_HEIGHT = 8000;

private:
    static bool TryParseProtocolVersion(Frame* parentFrame, int packetStartIndex, int packetEndIndex, QString& protocolVersionString, int& bytesParsed);
    static bool TryGetServerSecurityTypes(Frame* parentFrame, int packetStartIndex, int packetEndIndex, QVector<uint8_t>& securityTypes, int& bytesParsed);
    static bool TryGetClientSecurityType(Frame* parentFrame, int packetStartIndex, int packetEndIndex, uint8_t& securityType, int& bytesParsed);
};

class VncResponsePacket : public AbstractPacket {
public:
    enum class VncResponseCode : uint8_t {
        FramebufferUpdate = 0,
        SetColourMapEntries = 1,
        Bell = 2,
        ServerCutText = 3
    };

    static bool TryParseServerInit(const QByteArray& data, int& offset, uint16_t& width, uint16_t& height, RfbPacket::VncPixelFormat& pixelFormat, QString& desktopName);

    VncResponseCode ResponseCode;

    VncResponsePacket(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool isClientToServer);
    QVector<std::shared_ptr<AbstractPacket>> GetSubPackets(bool includeSelfReference) override;
};

class VncCommandPacket : public AbstractPacket {
public:
    enum class VncCommand : uint8_t {
        SetPixelFormat = 0,
        FixColourMapEntries = 1,
        SetEncodings = 2,
        FramebufferUpdateRequest = 3,
        KeyEvent = 4,
        PointerEvent = 5,
        ClientCutText = 6,
    };

    VncCommand Command;
    std::optional<RfbPacket::VncPixelFormat> PixelFormat;

    VncCommandPacket(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);
    QVector<std::shared_ptr<AbstractPacket>> GetSubPackets(bool includeSelfReference) override;
};

class KeyEventPacket : public VncCommandPacket {
public:
    enum class SpecialKeys : uint32_t {
        BackSpace = 0xff08, Tab = 0xff09, Return = 0xff0d, Escape = 0xff1b, Insert = 0xff63, Delete = 0xffff,
        Home = 0xff50, End = 0xff57, Page_Up = 0xff55, Page_Down = 0xff56, Left = 0xff51, Up = 0xff52,
        Right = 0xff53, Down = 0xff54, F1 = 0xffbe, F2 = 0xffbf, F3 = 0xffc0, F4 = 0xffc1, F5 = 0xffc1,
        F6 = 0xffc2, F7 = 0xffc3, F8 = 0xffc4, F9 = 0xffc5, F10 = 0xffc6, F11 = 0xffc7, F12 = 0xffc9,
        Shift_left = 0xffe1, Shift_right = 0xffe2, Control_left = 0xffe3, Control_right = 0xffe4,
        Meta_left = 0xffe7, Meta_right = 0xffe8, Alt_left = 0xffe9, Alt_right = 0xffea,
        KeyPad_Enter = 0xFF8D, KeyPad_Home = 0xFF95, KeyPad_Left = 0xFF96, KeyPad_Up = 0xFF97, KeyPad_Right = 0xFF98,
        KeyPad_Down = 0xFF99, KeyPad_Page_Up = 0xFF9A, KeyPad_Page_Down = 0xFF9B, KeyPad_End = 0xFF9C, KeyPad_Begin = 0xFF9D,
        KeyPad_Insert = 0xFF9E, KeyPad_Delete = 0xFF9F, KeyPad_Equal = 0xFFBD, KeyPad_0 = 0xFFB0, KeyPad_1 = 0xFFB1,
        KeyPad_2 = 0xFFB2, KeyPad_3 = 0xFFB3, KeyPad_4 = 0xFFB4, KeyPad_5 = 0xFFB5, KeyPad_6 = 0xFFB6, KeyPad_7 = 0xFFB7,
        KeyPad_8 = 0xFFB8, KeyPad_9 = 0xFFB9, KeyPad_Decimal = 0xFFAE, KeyPad_Add = 0xFFAB, KeyPad_Subtract = 0xFFAD,
        KeyPad_Multiply = 0xFFAA, KeyPad_Divide = 0xFFAF, Select = 0xFF60, Print = 0xFF61, Execute = 0xFF62,
        Undo = 0xFF65, Redo = 0xFF66, Menu = 0xFF67, Find = 0xFF68, Cancel = 0xFF69, Help = 0xFF6A, Break = 0xFF6B,
        Mode_switch = 0xFF7E, script_switch = 0xFF7E, Num_Lock = 0xFF7F,
    };

    QString Key;
    bool Down;

    KeyEventPacket(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);
    QString ToString() const;
};

class FixColorMapPacket : public VncCommandPacket {
public:
    static bool TryParse(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, std::shared_ptr<FixColorMapPacket>& cmPacket);
    uint16_t NumberOfColors;
private:
    FixColorMapPacket(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);
};

class SetPixelFormatPacket : public VncCommandPacket {
public:
    static bool TryParse(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, std::shared_ptr<SetPixelFormatPacket>& packet, bool requireZeroValuePadding = true);
private:
    SetPixelFormatPacket(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);
};

class SetEncodingsPacket : public VncCommandPacket {
public:
    static bool TryParse(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, std::shared_ptr<SetEncodingsPacket>& packet);
    uint16_t NumberOfEncodings;
    QVector<int32_t> Encodings;
private:
    SetEncodingsPacket(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);
};

class FrameBufferUpdateRequestPacket : public VncCommandPacket {
public:
    static bool TryParse(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, std::shared_ptr<FrameBufferUpdateRequestPacket>& packet);
    bool Incremental;
    uint16_t X;
    uint16_t Y;
    uint16_t Width;
    uint16_t Height;
private:
    FrameBufferUpdateRequestPacket(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);
};

class ClientCutTextPacket : public VncCommandPacket {
public:
    static bool TryParse(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, std::shared_ptr<ClientCutTextPacket>& packet);
    QString Text;
private:
    ClientCutTextPacket(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);
};

class IRectangleData {
public:
    virtual ~IRectangleData() = default;
    virtual int Length() const = 0;
    virtual QByteArray ImageBytes() const = 0;
};

class Rectangle {
public:
    enum class FrameBufferEncoding : int32_t {
        Raw = 0, CopyRect = 1, RRE = 2, Hextile = 5, Tight = 7, TRLE = 15, ZRLE = 16, Zlib = 6,
        CursorPseudoEncoding = -239, DesktopSizePseudoEncoding = -223,
        ExtendedDesktopSizePseudoEncoding = -308, XCursorPseudoEncoding = -240,
        DesktopNamePseudoEncoding = -307, ExtendedClipboardPseudoEncoding = -305,
        LastRectPseudoEncoding = -224, CursorWithAlphaPseudoEncoding = -311,
        QEMU_PointerMotionChange = -257, QEMU_ExtendedKeyEvent = -258,
        QEMU_Audio = -259, QEMU_LED_State = -261
    };

    uint16_t X;
    uint16_t Y;
    uint16_t Width;
    uint16_t Height;
    int32_t EncodingRaw;
    std::shared_ptr<IRectangleData> RectangleData;
    int ParsedBytes;
    int RectangleDataLength = -1;

    int TotalLenght() const;
    bool TryGetEncoding(FrameBufferEncoding& encoding) const;
    static bool TryParse(const QByteArray& data, int offset, int maxLength, std::optional<RfbPacket::VncPixelFormat> pf, std::shared_ptr<Rectangle>& rect);

private:
    Rectangle() = default;
};

class BasicRectangleData : public IRectangleData {
public:
    QByteArray m_ImageBytes;
    int Length() const override { return m_ImageBytes.size(); }
    QByteArray ImageBytes() const override { return m_ImageBytes; }
};

class XCursorRectangleData : public IRectangleData {
public:
    QByteArray m_ImageBytes;
    int Length() const override { return m_ImageBytes.size(); }
    QByteArray ImageBytes() const override { return m_ImageBytes; }
};

class CursorRectangleData : public IRectangleData {
public:
    QByteArray m_ImageBytes;
    int Length() const override { return m_ImageBytes.size(); }
    QByteArray ImageBytes() const override { return m_ImageBytes; }
};

class ZlibRectangleData : public IRectangleData {
public:
    QByteArray m_ImageBytes;
    int Length() const override { return m_ImageBytes.size(); }
    QByteArray ImageBytes() const override { return m_ImageBytes; }
};

class TightRectangleData : public IRectangleData {
public:
    enum class CompressionMethod : uint8_t {
        Basic = 0, Fill = 8, Jpeg = 9
    };
    uint8_t Compression;
    QByteArray m_ImageBytes;
    int Length() const override { return m_ImageBytes.size() + 1; }
    QByteArray ImageBytes() const override { return m_ImageBytes; }
};

class ServerCutTextPacket : public VncResponsePacket {
public:
    static bool TryParse(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, std::shared_ptr<ServerCutTextPacket>& cutText);
    QString Text;
private:
    ServerCutTextPacket(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);
};

class FrameBufferUpdatePacket : public VncResponsePacket {
public:
    static bool TryParse(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer, bool includeColorMapWorkaround, std::shared_ptr<FrameBufferUpdatePacket>& updatePacket);
    uint16_t NumberOfRectangles;
    QVector<std::shared_ptr<Rectangle>> Rectangles;
private:
    FrameBufferUpdatePacket(Frame* parentFrame, int packetStartIndex, int packetEndIndex, bool clientToServer);
};

} // namespace Packets
} // namespace PacketParser
