#include "Md5SingletonHelper.hpp"
#include <QCryptographicHash>
#include <QByteArray>
#include <stdexcept>

namespace SharedUtils {

std::string Md5SingletonHelper::GetMd5HashString(const std::vector<uint8_t>& data) {
    if (data.empty()) return "";
    QByteArray hash = QCryptographicHash::hash(
        QByteArray(reinterpret_cast<const char*>(data.data()), data.size()), 
        QCryptographicHash::Md5
    );
    return hash.toHex().toStdString();
}

std::string Md5SingletonHelper::GetMd5HashString(const std::vector<uint8_t>& data, int offset, int count) {
    if (offset < 0 || count < 0 || offset + count > static_cast<int>(data.size())) {
        throw std::out_of_range("Invalid offset or count");
    }
    if (count == 0) return "";
    QByteArray hash = QCryptographicHash::hash(
        QByteArray(reinterpret_cast<const char*>(data.data() + offset), count), 
        QCryptographicHash::Md5
    );
    return hash.toHex().toStdString();
}

}
