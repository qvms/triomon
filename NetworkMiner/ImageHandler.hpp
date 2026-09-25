#pragma once

#include <QList>
#include <QSize>
#include <QString>
#include <memory>
#include <QListWidget>

// Placeholder for now as this is a UI class, just ensuring it has a translation stub
namespace PacketHandlerFramework {
namespace FileTransfer {
    class ReconstructedFile; // forward declaration
}
}
class QPixmap;

namespace NetworkMiner {

class ImageHandler {
private:
    static const int DEFAULT_IMAGE_DIMENSION = 64;
    static const int MIN_ZOOM_LEVEL = 10;
    static const int MAX_ZOOM_LEVEL = 100 * 256 / 64;

    int zoomLevel = 100;
    QString activeFilenameFilter;
    int activeMinPixelsFilter = 0;
    QListWidget* parentListWidget;

public:
    ImageHandler(QListWidget* parentListWidget);

    int ZoomLevel() const { return zoomLevel; }
    void SetZoomLevel(int value);
    
    int ImageCount() const;

    void ResetZoomLevel();
    void Clear();
};

} // namespace NetworkMiner
