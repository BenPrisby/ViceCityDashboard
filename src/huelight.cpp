#include "huelight.h"

#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

namespace {
constexpr int MIN_CAPABLE_BRIGHTNESS = 1;
constexpr int MAX_CAPABLE_BRIGHTNESS = 254;
}  // namespace
/*--------------------------------------------------------------------------------------------------------------------*/

HueLight::HueLight(int id, QObject* parent) : HueDevice(id, parent) {
    // Nothing else to do.
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueLight::commandBrightness(const double brightness) {
    if (qIsNaN(brightness) || (brightness < 0.0) || (brightness > 100.0)) {
        qDebug() << "Ignoring request to set invalid brightness for light with ID: " << id_;
        return;
    }

    // If the light is not on, turn it on or else the command will fail.
    if (!isOn_) {
        commandPower(true);
    }

    // Scale from a percentage into the capable range of the light.
    int scaledBrightness =
        qRound(((brightness / 100.0) * (MAX_CAPABLE_BRIGHTNESS - MIN_CAPABLE_BRIGHTNESS)) + MIN_CAPABLE_BRIGHTNESS);
    VCHub::instance()->hue()->commandDeviceState(id_, QJsonObject{{"bri", scaledBrightness}});
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueLight::handleStateData(const QJsonObject& state) {
    // Call the parent.
    HueDevice::handleStateData(state);

    if (state.contains("bri")) {
        // Present the brightness as a percentage.
        double brightness = ((state.value("bri").toDouble() - MIN_CAPABLE_BRIGHTNESS) /
                             (MAX_CAPABLE_BRIGHTNESS - MIN_CAPABLE_BRIGHTNESS)) *
                            100.0;
        if (brightness_ != brightness) {
            brightness_ = brightness;
            emit brightnessChanged();
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
