#ifndef HUECOLORLIGHT_H_
#define HUECOLORLIGHT_H_

#include "hueambiancelight.h"

class HueColorLight final : public HueAmbianceLight {
    Q_OBJECT

    // clang-format off
    Q_PROPERTY(QColor color  READ color  NOTIFY colorChanged)
    Q_PROPERTY(int hue       READ hue    NOTIFY colorChanged)
    // clang-format on

 public:
    explicit HueColorLight(int id, QObject* parent = nullptr);

    const QColor& color() const { return color_; }
    int hue() const { return color_.hue(); }

    static QColor xyToColor(double x, double y);
    static QColor hueToColor(int hue);

 signals:
    void colorChanged();

 public slots:
    void commandColor(const QColor& color);
    void commandColor(int hue);
    void commandColor(double x, double y);

 private:
    QColor color_;

    void handleStateData(const QJsonObject& state) override;

    Q_DISABLE_COPY_MOVE(HueColorLight)
};

#endif  // HUECOLORLIGHT_H_
