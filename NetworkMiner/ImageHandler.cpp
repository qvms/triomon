#include "ImageHandler.hpp"
#include <algorithm>

namespace NetworkMiner {

ImageHandler::ImageHandler(QListWidget* parentListWidget)
    : parentListWidget(parentListWidget) {
}

void ImageHandler::SetZoomLevel(int value) {
    if (value != zoomLevel) {
        if (value < MIN_ZOOM_LEVEL) {
            zoomLevel = MIN_ZOOM_LEVEL;
        } else if (value > MAX_ZOOM_LEVEL) {
            zoomLevel = MAX_ZOOM_LEVEL;
        } else {
            zoomLevel = value;
            // set image size logic
        }
    }
}

int ImageHandler::ImageCount() const {
    return 0; // return reconstructedImages.size();
}

void ImageHandler::ResetZoomLevel() {
    // SetImageSize(DEFAULT_IMAGE_SIZE);
}

void ImageHandler::Clear() {
    // reconstructedImages.clear();
    if(parentListWidget) {
        parentListWidget->clear();
    }
}

} // namespace NetworkMiner
