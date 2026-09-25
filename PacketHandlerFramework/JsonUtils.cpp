#include "JsonUtils.hpp"
#include <QByteArray>
#include <QJsonParseError>
#include <zlib.h>

namespace PacketHandlerFramework {

static QByteArray DecompressGzip(const QByteArray& data) {
    if (data.isEmpty()) return QByteArray();

    QByteArray out;
    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = data.size();
    strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data.data()));

    // 15 + 16 for gzip
    int ret = inflateInit2(&strm, 15 + 16);
    if (ret != Z_OK) return QByteArray();

    char outBuffer[32768];
    do {
        strm.avail_out = sizeof(outBuffer);
        strm.next_out = reinterpret_cast<Bytef*>(outBuffer);
        ret = inflate(&strm, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            inflateEnd(&strm);
            return QByteArray();
        }
        int have = sizeof(outBuffer) - strm.avail_out;
        out.append(outBuffer, have);
    } while (strm.avail_out == 0);

    inflateEnd(&strm);
    return out;
}

std::map<std::string, std::vector<std::string>> JsonUtils::GetParams(const std::vector<uint8_t>& data, bool gzip, int maxChildrenPerItem, bool& elementsHaveBeenSkipped) {
    elementsHaveBeenSkipped = false;
    QByteArray qData(reinterpret_cast<const char*>(data.data()), data.size());
    
    if (gzip) {
        qData = DecompressGzip(qData);
    }

    if (qData.isEmpty()) {
        return {};
    }

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(qData, &err);
    if (err.error != QJsonParseError::NoError) {
        return {};
    }

    return GetParams(doc, maxChildrenPerItem, elementsHaveBeenSkipped);
}

std::map<std::string, std::vector<std::string>> JsonUtils::GetParams(const QJsonDocument& jsonDoc, int maxChildrenPerItem, bool& elementsHaveBeenSkipped) {
    std::map<std::string, std::vector<std::string>> elements;
    
    if (jsonDoc.isObject()) {
        TraverseJson(QJsonValue(jsonDoc.object()), "", elements, maxChildrenPerItem, elementsHaveBeenSkipped);
    } else if (jsonDoc.isArray()) {
        TraverseJson(QJsonValue(jsonDoc.array()), "", elements, maxChildrenPerItem, elementsHaveBeenSkipped);
    }

    return elements;
}

void JsonUtils::TraverseJson(const QJsonValue& val, const QString& parentKey, std::map<std::string, std::vector<std::string>>& elements, int maxChildrenPerItem, bool& elementsHaveBeenSkipped) {
    if (val.isObject()) {
        QJsonObject obj = val.toObject();
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            TraverseJson(it.value(), it.key(), elements, maxChildrenPerItem, elementsHaveBeenSkipped);
        }
    } else if (val.isArray()) {
        QJsonArray arr = val.toArray();
        for (const QJsonValue& v : arr) {
            // Arrays in the C# code propagate the parent name
            TraverseJson(v, parentKey, elements, maxChildrenPerItem, elementsHaveBeenSkipped);
        }
    } else {
        if (!parentKey.isEmpty()) {
            QString strVal = val.toVariant().toString();
            if (!strVal.isEmpty()) {
                // In C# it checked for comma, brace, bracket
                int childCount = 0;
                for (QChar c : strVal) {
                    if (c == ',' || c == '{' || c == '[') {
                        childCount++;
                    }
                }

                if (maxChildrenPerItem < 0 || childCount <= maxChildrenPerItem) {
                    std::string key = parentKey.toStdString();
                    std::string value = strVal.toStdString();
                    
                    auto& list = elements[key];
                    if (std::find(list.begin(), list.end(), value) == list.end()) {
                        list.push_back(value);
                    }
                } else {
                    elementsHaveBeenSkipped = true;
                }
            }
        }
    }
}

} // namespace PacketHandlerFramework
