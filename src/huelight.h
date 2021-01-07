#ifndef HUELIGHT_H_
#define HUELIGHT_H_

#include "huedevice.h"

class HueLight : public HueDevice
{
    Q_OBJECT
    Q_PROPERTY( double brightness  READ brightness  NOTIFY brightnessChanged )

public:
    explicit HueLight( int iID, QObject * pParent = nullptr );

    double brightness() const { return m_dBrightness; }

signals:
    void brightnessChanged();

public slots:
    void commandBrightness( double dBrightness );

protected:
    double m_dBrightness;

    void handleStateData( const QJsonObject & State ) override;

private:
    Q_DISABLE_COPY_MOVE( HueLight )
};

#endif  // HUELIGHT_H_
