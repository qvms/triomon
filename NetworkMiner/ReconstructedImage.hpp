#pragma once

#include <QString>
#include <QImage>
#include <QPixmap>
#include <QSize>
#include <memory>
#include "PacketHandlerFramework/FileTransfer/ReconstructedFile.hpp"

namespace NetworkMiner {

class ReconstructedImage {
private:
    static const int MAX_CACHED_PIXEL_COUNT = 40000; // 200x200
    QPixmap cachedBitmap;
    QSize imageSize;
    std::shared_ptr<PacketHandlerFramework::FileTransfer::ReconstructedFile> imageFile;

    bool TrySetBitmap(const QImage& bitmap);
    
public:
    explicit ReconstructedImage(std::shared_ptr<PacketHandlerFramework::FileTransfer::ReconstructedFile> file, const QImage& bitmap = QImage());
    ~ReconstructedImage() = default;

    std::shared_ptr<PacketHandlerFramework::FileTransfer::ReconstructedFile> ImageFile() const { return imageFile; }
    QString GetText() const;
    int CountPixels(const QImage& bm = QImage());
    QSize GetImageSize(const QImage& bm = QImage());
    QPixmap GetBitmap();
    bool TryGetCachedBitmap(QPixmap& bitmap) const;
    QString GetDescription() const;
};

} // namespace NetworkMiner
