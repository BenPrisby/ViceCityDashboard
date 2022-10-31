#include "vcplugin.h"

#include <QDebug>
/*--------------------------------------------------------------------------------------------------------------------*/

VCPlugin::VCPlugin(const QString& name, QObject* parent)
    : QObject(parent), pluginName_(name), updateInterval_(10 * 1000), isActive_(true) {
    if (!pluginName_.isEmpty()) {
        setObjectName(pluginName_);
        qDebug() << "Initializing plugin: " << pluginName_;

        // Configure the update timer for periodically refreshing any attached data.
        updateTimer_.setInterval(updateInterval_);
        updateTimer_.setSingleShot(false);
        connect(&updateTimer_, &QTimer::timeout, this, &VCPlugin::refresh);
        updateTimer_.start();
    } else {
        qFatal("Missing name for VCPlugin");
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCPlugin::setUpdateInterval(const int value) {
    if (updateInterval_ != value) {
        updateInterval_ = value;
        updateTimer_.setInterval(updateInterval_);
        emit updateIntervalChanged();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCPlugin::setActive(const bool value) {
    if (isActive_ != value) {
        isActive_ = value;
        emit isActiveChanged();

        if (isActive_) {
            updateTimer_.start();
            refresh();
        } else {
            updateTimer_.stop();
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
