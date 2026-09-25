#include "RfbRectangleAssembler.hpp"

namespace PacketHandlerFramework {
namespace FileTransfer {

RfbRectangleAssembler::RfbRectangleAssembler(PacketParser::Packets::RfbPacket::FrameBufferUpdatePacket* fbUpdatePacket, 
                                             uint32_t startSequenceNumber, bool clientToServer, 
                                             PacketParser::NetworkTcpSession* session, 
                                             PacketParser::Packets::RfbPacket::VncPixelFormat* pixelFormat)
    : startSequenceNumber(startSequenceNumber),
      tcpDataStream(startSequenceNumber, 0, 0), // Use stub ports since we don't have session port access here right now
      pixelFormat(pixelFormat),
      session(session),
      totalRectangles(-1) {
    
    if (fbUpdatePacket && fbUpdatePacket->ParentFrame()) {
        this->startTime = fbUpdatePacket->ParentFrame()->Timestamp();
        this->startFrameNumber = fbUpdatePacket->ParentFrame()->FrameNumber();
    }
}

RfbRectangleAssembler::~RfbRectangleAssembler() {
    qDeleteAll(parsedRectangles);
}

bool RfbRectangleAssembler::IsCompleted() const {
    return totalRectangles >= 0 && parsedRectangles.count() >= totalRectangles;
}

void RfbRectangleAssembler::addData(const QByteArray& data, quint32 sequenceNumber) {
    if (data.length() > 0) {
        tcpDataStream.AddTcpData(sequenceNumber, data);
    }
    
    if (tcpDataStream.CountBytesToRead() >= PacketParser::Packets::RfbPacket::Rectangle::HEADER_LENGTH) {
        auto vtd = tcpDataStream.GetAllAvailableTcpData();
        QByteArray reassembledBytes = vtd.GetBytes(false);
        int offset = 0;
        
        PacketParser::Packets::RfbPacket::Rectangle* rectangle = new PacketParser::Packets::RfbPacket::Rectangle();
        
        while ((totalRectangles < 0 || parsedRectangles.count() < totalRectangles) && 
               PacketParser::Packets::RfbPacket::Rectangle::TryParse(reassembledBytes, offset, pixelFormat, true, rectangle)) {
            
            parsedRectangles.append(rectangle);
            offset += rectangle->TotalLenght;
            tcpDataStream.RemoveData(rectangle->TotalLenght);
            
            PacketParser::Packets::RfbPacket::Rectangle::FrameBufferEncoding encoding;
            if (rectangle->TryGetEncoding(&encoding)) {
                if (encoding == PacketParser::Packets::RfbPacket::Rectangle::LastRect) {
                    totalRectangles = parsedRectangles.count();
                }
            }
            
            rectangle = new PacketParser::Packets::RfbPacket::Rectangle();
        }
        delete rectangle; // cleanup last unparsed instance
    }
    
    // In C# it returned int (processed bytes length). Interface uses void addData though.
}

void RfbRectangleAssembler::finishAssembling() {
    if (parsedRectangles.count() > 0 && OnFinish) {
        OnFinish(session, startFrameNumber, startTime, parsedRectangles);
    }
}

void RfbRectangleAssembler::Clear() {
    tcpDataStream.Clear();
}

} // namespace FileTransfer
} // namespace PacketHandlerFramework
