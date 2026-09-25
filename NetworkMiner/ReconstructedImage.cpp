#include "ReconstructedImage.hpp"
#include <QFileInfo>

namespace NetworkMiner {

ReconstructedImage::ReconstructedImage(std::shared_ptr<PacketHandlerFramework::FileTransfer::ReconstructedFile> file, const QImage& bitmap)
    : imageFile(file) {
    if (!bitmap.isNull()) {
        TrySetBitmap(bitmap);
    }
}

bool ReconstructedImage::TrySetBitmap(const QImage& bitmap) {
    if (bitmap.isNull()) return false;
    
    if (cachedBitmap.isNull() && CountPixels(bitmap) <= MAX_CACHED_PIXEL_COUNT) {
        cachedBitmap = QPixmap::fromImage(bitmap);
        return true;
    }
    return false;
}

QString ReconstructedImage::GetText() const {
    QSize sz = const_cast<ReconstructedImage*>(this)->GetImageSize();
    return imageFile->Filename() + "\n" + QString::number(sz.width()) + "x" + QString::number(sz.height()) + ", " + imageFile->FileSizeString();
}

int ReconstructedImage::CountPixels(const QImage& bm) {
    QSize size = GetImageSize(bm);
    return size.width() * size.height();
}

QSize ReconstructedImage::GetImageSize(const QImage& bm) {
    if (imageSize.isValid()) {
        return imageSize;
    }
    
    if (!bm.isNull()) {
        imageSize = bm.size();
    } else {
        imageSize = GetBitmap().size();
    }
    return imageSize;
}

QPixmap ReconstructedImage::GetBitmap() {
    if (!cachedBitmap.isNull()) {
        return cachedBitmap;
    }
    
    QImage img(imageFile->FilePath());
    TrySetBitmap(img);
    return QPixmap::fromImage(img);
}

bool ReconstructedImage::TryGetCachedBitmap(QPixmap& bitmap) const {
    if (cachedBitmap.isNull()) {
        return false;
    }
    bitmap = cachedBitmap;
    return true;
}

QString ReconstructedImage::GetDescription() const {
    // Assuming NetworkHost has a GetIPAddressString or similar representation. 
    // We will use simplified text for now.
    QString src = imageFile->SourceHost() ? "SourceHost" : "Unknown";
    QString dst = imageFile->DestinationHost() ? "DestHost" : "Unknown";
    return "Source: " + src + "\nDestination: " + dst + "\nReconstructed file path: " + imageFile->FilePath();
}

} // namespace NetworkMiner
