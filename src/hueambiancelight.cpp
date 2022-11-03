#include "hueambiancelight.h"

#include <QtMath>

#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

namespace {
constexpr int WARMEST_CAPABLE_CT = 500;
constexpr int COLDEST_CAPABLE_CT = 153;
const QHash<int, QColor> COLOR_TEMPERATURE_MAP = {
    {2000, QColor(240, 219, 131)}, {2100, QColor(241, 221, 137)}, {2200, QColor(242, 222, 142)},
    {2300, QColor(243, 224, 148)}, {2400, QColor(243, 225, 153)}, {2500, QColor(243, 228, 158)},
    {2600, QColor(244, 230, 164)}, {2700, QColor(245, 231, 169)}, {2800, QColor(246, 233, 176)},
    {2900, QColor(246, 233, 180)}, {3000, QColor(247, 235, 186)}, {3100, QColor(247, 236, 191)},
    {3200, QColor(248, 238, 197)}, {3300, QColor(249, 240, 203)}, {3400, QColor(249, 242, 208)},
    {3500, QColor(250, 244, 215)}, {3600, QColor(250, 245, 220)}, {3700, QColor(251, 246, 225)},
    {3800, QColor(252, 248, 232)}, {3900, QColor(252, 249, 237)}, {4000, QColor(253, 251, 243)},
    {4100, QColor(254, 252, 249)}, {4200, QColor(254, 254, 254)}, {4300, QColor(252, 253, 254)},
    {4400, QColor(250, 253, 254)}, {4500, QColor(247, 252, 254)}, {4600, QColor(245, 252, 253)},
    {4700, QColor(243, 251, 253)}, {4800, QColor(240, 250, 253)}, {4900, QColor(238, 250, 253)},
    {5000, QColor(236, 250, 253)}, {5100, QColor(233, 249, 253)}, {5200, QColor(231, 247, 252)},
    {5300, QColor(228, 247, 252)}, {5400, QColor(226, 246, 252)}, {5500, QColor(224, 247, 252)},
    {5600, QColor(221, 245, 251)}, {5700, QColor(219, 244, 251)}, {5800, QColor(217, 244, 251)},
    {5900, QColor(215, 243, 250)}, {6000, QColor(213, 243, 250)}, {6100, QColor(210, 242, 250)},
    {6200, QColor(208, 242, 250)}, {6300, QColor(206, 241, 250)}, {6400, QColor(204, 241, 250)},
    {6500, QColor(203, 241, 250)}};
}  // namespace
/*--------------------------------------------------------------------------------------------------------------------*/

HueAmbianceLight::HueAmbianceLight(int id, QObject* parent) : HueLight(id, parent), colorTemperature_(0) {
    // Nothing else to do.
}
/*--------------------------------------------------------------------------------------------------------------------*/

int HueAmbianceLight::minColorTemperature() const {
    return qRound(1.0e6 / WARMEST_CAPABLE_CT);
}
/*--------------------------------------------------------------------------------------------------------------------*/

int HueAmbianceLight::maxColorTemperature() const {
    return qRound(1.0e6 / COLDEST_CAPABLE_CT);
}
/*--------------------------------------------------------------------------------------------------------------------*/

QColor HueAmbianceLight::ambientColor() const {
    // Take the nearest reference point.
    int remainder = colorTemperature_ % 100;
    int reference = colorTemperature_ - remainder;
    if (remainder >= 50) {
        reference += 100;
    }

    return COLOR_TEMPERATURE_MAP.value(reference);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueAmbianceLight::commandColorTemperature(int colorTemperature) {
    if ((colorTemperature < minColorTemperature()) || (colorTemperature > maxColorTemperature())) {
        qDebug() << "Ignoring request to set invalid color temperature for light with ID: " << id_;
        return;
    }

    // If the light is not on, turn it on or else the command will fail.
    if (!isOn_) {
        commandPower(true);
    }

    // Scale from kelvins to mirek.
    colorTemperature = qRound(1.0e6 / colorTemperature);
    VCHub::instance()->hue()->commandDeviceState(id_, QJsonObject{{"ct", colorTemperature}});
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueAmbianceLight::handleStateData(const QJsonObject& state) {
    // Call the parent.
    HueLight::handleStateData(state);

    if (state.contains("ct")) {
        // Scale from mirek to kelvins.
        int colorTemperature = qRound(1.0e6 / state.value("ct").toInt());
        if (colorTemperature_ != colorTemperature) {
            colorTemperature_ = colorTemperature;
            emit colorTemperatureChanged();
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
