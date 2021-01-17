#ifndef HUECOLORLIGHT_H_
#define HUECOLORLIGHT_H_

#include "hueambiancelight.h"

class HueColorLight final : public HueAmbianceLight
{
    Q_OBJECT
    Q_PROPERTY( QColor color  READ color  NOTIFY colorChanged )
    Q_PROPERTY( int hue       READ hue    NOTIFY colorChanged )

public:
    explicit HueColorLight( int iID, QObject * pParent = nullptr );

    const QColor & color() const { return m_Color; }
    int hue() const { return m_Color.hue(); }

signals:
    void colorChanged();

public slots:
    void commandColor( const QColor & Color );
    void commandColor( int iHue );
    void commandColor( double dX, double dY );

private:
    QColor m_Color;

    void handleStateData( const QJsonObject & State ) override;

    Q_DISABLE_COPY_MOVE( HueColorLight )
};

#endif // HUECOLORLIGHT_H_
