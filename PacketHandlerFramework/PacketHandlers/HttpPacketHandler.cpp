#include "HttpPacketHandler.hpp"
#include "../../PacketHandlerFramework/MainPacketHandler.hpp"
#include "../../PacketHandlerFramework/Events/ParametersEventArgs.hpp"
#include "../../PacketHandlerFramework/FileTransfer/FileStreamAssembler.hpp"
#include <QVariantMap>
#include <QStringList>

namespace PacketHandlerFramework {
namespace PacketHandlers {

HttpPacketHandler::HttpPacketHandler(std::shared_ptr<MainPacketHandler> mainPacketHandler)
    : AbstractPacketHandler(mainPacketHandler) {
}

void HttpPacketHandler::ExtractDataFromPacket(void* transportPacket) {
}

int HttpPacketHandler::ExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, bool transferIsClientToServer, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    std::shared_ptr<PacketParser::NetworkHost> sourceHost = transferIsClientToServer ? tcpSession->GetClientHost() : tcpSession->GetServerHost();
    std::shared_ptr<PacketParser::NetworkHost> destinationHost = transferIsClientToServer ? tcpSession->GetServerHost() : tcpSession->GetClientHost();
    
    if (TryExtractData(tcpSession, sourceHost, destinationHost, packetList)) {
        int parsedBytes = 0;
        for (auto p : packetList) {
            auto hp = std::dynamic_pointer_cast<PacketParser::Packets::HttpPacket>(p);
            if(hp) {
                parsedBytes += hp->GetPacketLength();
            }
        }
        return parsedBytes;
    }
    return 0;
}

bool HttpPacketHandler::TryExtractData(std::shared_ptr<PacketParser::NetworkTcpSession> tcpSession, std::shared_ptr<PacketParser::NetworkHost> sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost, const QList<std::shared_ptr<PacketParser::Packets::AbstractPacket>>& packetList) {
    bool successfulExtraction = false;
    std::shared_ptr<PacketParser::Packets::HttpPacket> httpPacket;
    std::shared_ptr<PacketParser::Packets::TcpPacket> tcpPacket;
    
    for (auto& p : packetList) {
        if (!httpPacket) httpPacket = std::dynamic_pointer_cast<PacketParser::Packets::HttpPacket>(p);
        if (!tcpPacket) tcpPacket = std::dynamic_pointer_cast<PacketParser::Packets::TcpPacket>(p);
    }

    if (httpPacket && tcpPacket) {
        if (httpPacket->PacketHeaderIsComplete()) {
            if (httpPacket->RequestMethod() != PacketParser::Packets::HttpPacket::RequestMethods::POST || httpPacket->ContentLength() > 4096 || httpPacket->ContentIsComplete()) {
                ExtractHttpData(httpPacket, tcpPacket, sourceHost, destinationHost);
                successfulExtraction = true;
            }
        }
    }
    return successfulExtraction;
}

void HttpPacketHandler::ExtractHttpData(std::shared_ptr<PacketParser::Packets::HttpPacket> httpPacket, std::shared_ptr<PacketParser::Packets::TcpPacket> tcpPacket, std::shared_ptr<PacketParser::NetworkHost> sourceHost, std::shared_ptr<PacketParser::NetworkHost> destinationHost) {
    if (httpPacket->MessageTypeIsRequest()) {
        if (!httpPacket->UserAgentBanner().isEmpty())
            sourceHost->AddHttpUserAgentBanner(httpPacket->UserAgentBanner());
        if (!httpPacket->RequestedHost().isEmpty())
            destinationHost->AddHostName(httpPacket->RequestedHost(), httpPacket->GetPacketTypeDescription());
        if (!httpPacket->Cookie().isEmpty()) {
            QVariantMap cookieParams;
            QStringList cookies = httpPacket->Cookie().split(QRegularExpression("[;,]"), Qt::SkipEmptyParts);
            for (const QString& s : cookies) {
                QString cookieFragment = s.trimmed();
                int splitOffset = cookieFragment.indexOf('=');
                if (splitOffset > 0)
                    cookieParams.insert(cookieFragment.mid(0, splitOffset), cookieFragment.mid(splitOffset + 1));
                else
                    cookieParams.insert(cookieFragment, "");
            }
            auto ea = std::make_shared<PacketHandlerFramework::Events::ParametersEventArgs>(
                tcpPacket->GetParentFrame()->GetFrameNumber(), sourceHost, destinationHost, 
                "TCP " + QString::number(tcpPacket->SourcePort()), "TCP " + QString::number(tcpPacket->DestinationPort()), 
                cookieParams, httpPacket->GetParentFrame()->GetTimestamp(), "HTTP Cookie");
            GetMainPacketHandler()->OnParametersDetected(ea);
            
            auto credential = std::make_shared<PacketHandlerFramework::NetworkCredential>(
                sourceHost, destinationHost, "HTTP Cookie", httpPacket->Cookie(), "N/A", httpPacket->GetParentFrame()->GetTimestamp());
            GetMainPacketHandler()->AddCredential(credential);
        }
        if (!httpPacket->AuthorizationCredentialsUsername().isEmpty()) {
            auto nc = std::make_shared<PacketHandlerFramework::NetworkCredential>(
                sourceHost, destinationHost, httpPacket->GetPacketTypeDescription(), 
                httpPacket->AuthorizationCredentialsUsername(), httpPacket->AuthorizationCredentialsPassword(), 
                httpPacket->GetParentFrame()->GetTimestamp());
            GetMainPacketHandler()->AddCredential(nc);
        }

        if ((httpPacket->RequestMethod() == PacketParser::Packets::HttpPacket::RequestMethods::GET || httpPacket->RequestMethod() == PacketParser::Packets::HttpPacket::RequestMethods::POST) && !httpPacket->RequestedFileName().isEmpty()) {
            
            auto queryStringData = httpPacket->GetQuerystringData();
            if (!queryStringData.isEmpty()) {
                QVariantMap qsd;
                for (auto p : queryStringData) qsd.insert(p.first, p.second);
                auto ea = std::make_shared<PacketHandlerFramework::Events::ParametersEventArgs>(
                    tcpPacket->GetParentFrame()->GetFrameNumber(), sourceHost, destinationHost, 
                    "TCP " + QString::number(tcpPacket->SourcePort()), "TCP " + QString::number(tcpPacket->DestinationPort()), 
                    qsd, tcpPacket->GetParentFrame()->GetTimestamp(), "HTTP QueryString");
                GetMainPacketHandler()->OnParametersDetected(ea);
            }

            QString fileUri = httpPacket->RequestedFileName();
            QString queryString;
            if (fileUri.contains("?")) {
                if (fileUri.indexOf('?') + 1 < fileUri.length())
                    queryString = fileUri.mid(fileUri.indexOf('?') + 1);
                fileUri = fileUri.mid(0, fileUri.indexOf('?'));
            }
            if (fileUri.startsWith("http://"))
                fileUri = fileUri.mid(7);
            if (fileUri.startsWith("www.") && fileUri.contains("/"))
                fileUri = fileUri.mid(fileUri.indexOf("/"));

            QStringList uriParts = fileUri.split('/', Qt::SkipEmptyParts); // Simplified separators
            QString filename;
            QString fileLocation = "";

            if (fileUri.endsWith("/")) {
                filename = "index.html";
                for (int i = 0; i < uriParts.size(); i++)
                    if (!uriParts[i].isEmpty() && !uriParts[i].contains(".."))
                        fileLocation += "/" + uriParts[i];
            } else {
                if (!uriParts.isEmpty()) {
                    filename = uriParts.last();
                    for (int i = 0; i < uriParts.size() - 1; i++)
                        if (!uriParts[i].isEmpty() && !uriParts[i].contains(".."))
                            fileLocation += "/" + uriParts[i];
                }
            }

            if (!queryString.isEmpty()) {
                filename += "." + QString::number(qHash(queryString) & 0xFFFF, 16).toUpper().rightJustified(4, '0');
            }

            try {
                if (GetMainPacketHandler()->GetFileStreamAssemblerList()->ContainsAssembler(destinationHost, tcpPacket->DestinationPort(), sourceHost, tcpPacket->SourcePort(), true)) {
                    auto oldAssembler = GetMainPacketHandler()->GetFileStreamAssemblerList()->GetAssembler(destinationHost, tcpPacket->DestinationPort(), sourceHost, tcpPacket->SourcePort(), true);
                    GetMainPacketHandler()->GetFileStreamAssemblerList()->Remove(oldAssembler, true);
                }

                auto assembler = std::make_shared<PacketHandlerFramework::FileTransfer::FileStreamAssembler>(
                    GetMainPacketHandler()->GetFileStreamAssemblerList(), destinationHost, tcpPacket->DestinationPort(), sourceHost, tcpPacket->SourcePort(), tcpPacket != nullptr, PacketHandlerFramework::FileTransfer::FileStreamTypes::HttpGetNormal, filename, fileLocation, httpPacket->RequestedFileName(), httpPacket->GetParentFrame()->GetFrameNumber(), httpPacket->GetParentFrame()->GetTimestamp());
                GetMainPacketHandler()->GetFileStreamAssemblerList()->Add(assembler);
            } catch (...) {
                GetMainPacketHandler()->OnAnomalyDetected("Error creating assembler for HTTP file transfer");
            }

            if (httpPacket->RequestMethod() == PacketParser::Packets::HttpPacket::RequestMethods::POST) {
                if (!httpPacket->ContentType().isEmpty() && httpPacket->ContentType().startsWith("multipart/form-data")) {
                    try {
                        if (GetMainPacketHandler()->GetFileStreamAssemblerList()->ContainsAssembler(sourceHost, tcpPacket->SourcePort(), destinationHost, tcpPacket->DestinationPort(), true)) {
                            auto oldAssembler = GetMainPacketHandler()->GetFileStreamAssemblerList()->GetAssembler(sourceHost, tcpPacket->SourcePort(), destinationHost, tcpPacket->DestinationPort(), true);
                            GetMainPacketHandler()->GetFileStreamAssemblerList()->Remove(oldAssembler, true);
                        }

                        QString mimeBoundary;
                        if (httpPacket->ContentType().toLower().startsWith("multipart/form-data; boundary=") && httpPacket->ContentType().length() > 30) {
                            mimeBoundary = httpPacket->ContentType().mid(30);
                        } else {
                            mimeBoundary = "";
                        }

                        auto assembler = std::make_shared<PacketHandlerFramework::FileTransfer::FileStreamAssembler>(
                            GetMainPacketHandler()->GetFileStreamAssemblerList(), sourceHost, tcpPacket->SourcePort(), destinationHost, tcpPacket->DestinationPort(), tcpPacket != nullptr, PacketHandlerFramework::FileTransfer::FileStreamTypes::HttpPostMimeMultipartFormData, filename + ".form-data.mime", fileLocation, mimeBoundary, httpPacket->GetParentFrame()->GetFrameNumber(), httpPacket->GetParentFrame()->GetTimestamp());
                        assembler->SetFileContentLength(httpPacket->ContentLength());
                        // assembler->SetFileSegmentRemainingBytes(httpPacket->ContentLength());
                        GetMainPacketHandler()->GetFileStreamAssemblerList()->Add(assembler);
                        
                        if (assembler->TryActivate()) {
                            if (!httpPacket->MessageBody().empty()) {
                                QByteArray bodyBytes(reinterpret_cast<const char*>(httpPacket->MessageBody().data()), httpPacket->MessageBody().size());
                                assembler->AddData(bodyBytes, tcpPacket->SequenceNumber());
                            }
                        }
                    } catch (...) {
                        GetMainPacketHandler()->OnAnomalyDetected("Error creating assembler for HTTP file transfer");
                    }
                } else {
                    // ExtractMultipartFormData missing logic from c# handler
                }
            }
        }
    } else { // reply
        if (!httpPacket->ServerBanner().isEmpty())
            sourceHost->AddHttpServerBanner(httpPacket->ServerBanner(), tcpPacket->SourcePort());
        if (!httpPacket->WwwAuthenticateBasicRealm().isEmpty())
            sourceHost->AddHostName(httpPacket->WwwAuthenticateBasicRealm(), httpPacket->GetPacketTypeDescription());
        
        if (GetMainPacketHandler()->GetFileStreamAssemblerList()->ContainsAssembler(sourceHost, tcpPacket->SourcePort(), destinationHost, tcpPacket->DestinationPort(), true)) {
            auto assembler = GetMainPacketHandler()->GetFileStreamAssemblerList()->GetAssembler(sourceHost, tcpPacket->SourcePort(), destinationHost, tcpPacket->DestinationPort(), true);
            
            if (httpPacket->ContentLength() >= 0 || httpPacket->ContentLength() == -1) {
                assembler->SetFileContentLength(httpPacket->ContentLength());
                // assembler->SetFileSegmentRemainingBytes(httpPacket->ContentLength());
            }

            if (httpPacket->ContentLength() == 0) {
                GetMainPacketHandler()->GetFileStreamAssemblerList()->Remove(assembler, true);
            } else {
                if (!httpPacket->ContentType().isEmpty() && httpPacket->ContentType().contains("/") && httpPacket->ContentType().indexOf('/') < httpPacket->ContentType().length() - 1) {
                    QString extension = httpPacket->ContentType().mid(httpPacket->ContentType().indexOf('/') + 1);
                    if (extension.contains(";"))
                        extension = extension.mid(0, extension.indexOf(";"));
                    if (!extension.isEmpty() && 
                        !assembler->GetFilename().endsWith("." + extension, Qt::CaseInsensitive) &&
                        !(assembler->GetFilename().endsWith("jpg", Qt::CaseInsensitive) && extension.compare("jpeg", Qt::CaseInsensitive) == 0) &&
                        !(assembler->GetFilename().endsWith("htm", Qt::CaseInsensitive) && extension.compare("html", Qt::CaseInsensitive) == 0)) {
                        assembler->SetFilename(assembler->GetFilename() + "." + extension);
                    }
                }

                if (httpPacket->TransferEncoding() == "chunked")
                    assembler->SetFileStreamType(PacketHandlerFramework::FileTransfer::FileStreamTypes::HttpGetChunked);
                
                // ContentEncoding mapping - assuming HTTP packet doesn't expose Gzip enum currently, will skip parsing it here for now

                if (assembler->TryActivate()) {
                    if (!httpPacket->MessageBody().empty()) {
                        QByteArray bodyBytes(reinterpret_cast<const char*>(httpPacket->MessageBody().data()), httpPacket->MessageBody().size());
                        // && condition omitted because FileSegmentRemainingBytes isn't there yet
                        assembler->AddData(bodyBytes, tcpPacket->SequenceNumber());
                    }
                }
            }
        }
    }
}

void HttpPacketHandler::Reset() {
}

}
}
