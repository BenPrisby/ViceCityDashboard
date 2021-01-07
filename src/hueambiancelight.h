#ifndef HUEAMBIANCELIGHT_H_
#define HUEAMBIANCELIGHT_H_

#include <QColor>

#include "huelight.h"

class HueAmbianceLight : public HueLight
{
    Q_OBJECT
    Q_PROPERTY( int colorTemperature     READ colorTemperature     NOTIFY colorTemperatureChanged )
    Q_PROPERTY( int minColorTemperature  READ minColorTemperature  CONSTANT )
    Q_PROPERTY( int maxColorTemperature  READ maxColorTemperature  CONSTANT )
    Q_PROPERTY( QColor ambientColor      READ ambientColor         NOTIFY colorTemperatureChanged )

public:
    explicit HueAmbianceLight( int iID, QObject * pParent = nullptr );

    int colorTemperature() const { return m_iColorTemperature; }
    int minColorTemperature() const;
    int maxColorTemperature() const;
    QColor ambientColor() const;

signals:
    void colorTemperatureChanged();

public slots:
    void commandColorTemperature( int iColorTemperature );

protected:
    int m_iColorTemperature;

    void handleStateData( const QJsonObject & State ) override;

private:
    Q_DISABLE_COPY_MOVE( HueAmbianceLight )
};

#endif  // HUEAMBIANCELIGHT_H_
