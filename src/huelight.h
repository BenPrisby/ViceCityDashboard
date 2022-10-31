#ifndef HUELIGHT_H_
#define HUELIGHT_H_

#include "huedevice.h"

class HueLight : public HueDevice {
    Q_OBJECT

    // clang-format off
    Q_PROPERTY(double brightness  READ brightness  NOTIFY brightnessChanged)
    // clang-format on

 public:
    explicit HueLight(int id, QObject* parent = nullptr);

    double brightness() const { return brightness_; }

    void commandBrightness(double brightness);

 signals:
    void brightnessChanged();

 protected:
    double brightness_;

    void handleStateData(const QJsonObject& state) override;

 private:
    Q_DISABLE_COPY_MOVE(HueLight)
};

#endif  // HUELIGHT_H_
