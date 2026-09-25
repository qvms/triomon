#pragma once

#include <QString>
#include <QColor>
#include <memory>

namespace NetworkMiner {

class GuiProperties {
public:
    static const QString ISO_DATE_TIME_FORMAT;
    static const QString ISO_DATE_TIME_FORMAT_UTC_OFFSET_OUTPUT;

    GuiProperties() {
        AdvertisementColor = QColor(Qt::red);
        InternetTrackerColor = QColor(Qt::blue);
    }

    std::shared_ptr<GuiProperties> Clone() const {
        auto clone = std::make_shared<GuiProperties>();
        clone->AutomaticallyResizeColumnsWhenParsingComplete = this->AutomaticallyResizeColumnsWhenParsingComplete;
        clone->ColumnAutoResizeMaxWidth = this->ColumnAutoResizeMaxWidth;
        clone->AdvertisementColor = this->AdvertisementColor;
        clone->InternetTrackerColor = this->InternetTrackerColor;
        clone->UseVoipTab = this->UseVoipTab;
        clone->UseHostsTab = this->UseHostsTab;
        return clone;
    }

    bool AutomaticallyResizeColumnsWhenParsingComplete = true;
    uint16_t ColumnAutoResizeMaxWidth = 300;

    QColor AdvertisementColor;
    QString getAdvertismentColorHtml() const { return AdvertisementColor.name(); }
    void setAdvertismentColorHtml(const QString& val) { AdvertisementColor = QColor(val); }

    QColor InternetTrackerColor;
    QString getInternetTrackerColorHtml() const { return InternetTrackerColor.name(); }
    void setInternetTrackerColorHtml(const QString& val) { InternetTrackerColor = QColor(val); }

    bool UseVoipTab = true;
    bool UseHostsTab = true;
};

// Define constants in cpp (we'll just do it here with inline if C++17, but project uses C++20)
inline const QString GuiProperties::ISO_DATE_TIME_FORMAT = "yyyy-MM-dd HH:mm:ss";
inline const QString GuiProperties::ISO_DATE_TIME_FORMAT_UTC_OFFSET_OUTPUT = "yyyy-MM-dd HH:mm:ss UTCzz";

} // namespace NetworkMiner
