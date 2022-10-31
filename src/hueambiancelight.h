#ifndef HUEAMBIANCELIGHT_H_
#define HUEAMBIANCELIGHT_H_

#include <QColor>

#include "huelight.h"

class HueAmbianceLight : public HueLight {
    Q_OBJECT

    // clang-format off
    Q_PROPERTY(int colorTemperature     READ colorTemperature     NOTIFY colorTemperatureChanged)
    Q_PROPERTY(int minColorTemperature  READ minColorTemperature  CONSTANT)
    Q_PROPERTY(int maxColorTemperature  READ maxColorTemperature  CONSTANT)
    Q_PROPERTY(QColor ambientColor      READ ambientColor         NOTIFY colorTemperatureChanged)
    // clang-format on

 public:
    explicit HueAmbianceLight(int id, QObject* parent = nullptr);

    int colorTemperature() const { return colorTemperature_; }
    int minColorTemperature() const;
    int maxColorTemperature() const;
    QColor ambientColor() const;

 signals:
    void colorTemperatureChanged();

 public slots:
    void commandColorTemperature(int colorTemperature);

 protected:
    int colorTemperature_;

    void handleStateData(const QJsonObject& state) override;

 private:
    Q_DISABLE_COPY_MOVE(HueAmbianceLight)
};

#endif  // HUEAMBIANCELIGHT_H_
