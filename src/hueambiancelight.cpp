#include <QtMath>

#include "hueambiancelight.h"
#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

static constexpr int MIN_CAPABLE_CT_K = 2200;
static constexpr int MIN_CAPABLE_CT = 500;
static constexpr int MAX_CAPABLE_CT_K = 6500;
static constexpr int MAX_CAPABLE_CT = 153;
static const QHash<int, QColor> COLOR_TEMPERATURE_MAP = {
    { 2200, QColor( 255, 147, 44 ) },  { 2300, QColor( 255, 152, 54 ) },  { 2400, QColor( 255, 157, 63 ) },
    { 2500, QColor( 255, 161, 72 ) },  { 2600, QColor( 255, 165, 79 ) },  { 2700, QColor( 255, 169, 87 ) },
    { 2800, QColor( 255, 173, 94 ) },  { 2900, QColor( 255, 177, 101 ) }, { 3000, QColor( 255, 180, 107 ) },
    { 3100, QColor( 255, 184, 114 ) }, { 3200, QColor( 255, 187, 120 ) }, { 3300, QColor( 255, 190, 126 ) },
    { 3400, QColor( 255, 193, 132 ) }, { 3500, QColor( 255, 196, 137 ) }, { 3600, QColor( 255, 199, 143 ) },
    { 3700, QColor( 255, 201, 148 ) }, { 3800, QColor( 255, 204, 153 ) }, { 3900, QColor( 255, 206, 159 ) },
    { 4000, QColor( 255, 209, 163 ) }, { 4100, QColor( 255, 211, 168 ) }, { 4200, QColor( 255, 213, 173 ) },
    { 4300, QColor( 255, 215, 177 ) }, { 4400, QColor( 255, 217, 182 ) }, { 4500, QColor( 255, 219, 186 ) },
    { 4600, QColor( 255, 221, 190 ) }, { 4700, QColor( 255, 223, 194 ) }, { 4800, QColor( 255, 225, 198 ) },
    { 4900, QColor( 255, 227, 202 ) }, { 5000, QColor( 255, 228, 206 ) }, { 5100, QColor( 255, 230, 210 ) },
    { 5200, QColor( 255, 232, 213 ) }, { 5300, QColor( 255, 233, 217 ) }, { 5400, QColor( 255, 235, 220 ) },
    { 5500, QColor( 255, 236, 224 ) }, { 5600, QColor( 255, 238, 227 ) }, { 5700, QColor( 255, 239, 230 ) },
    { 5800, QColor( 255, 240, 233 ) }, { 5900, QColor( 255, 242, 236 ) }, { 6000, QColor( 255, 243, 239 ) },
    { 6100, QColor( 255, 244, 242 ) }, { 6200, QColor( 255, 245, 245 ) }, { 6300, QColor( 255, 246, 247 ) },
    { 6400, QColor( 255, 248, 251 ) }, { 6500, QColor( 255, 249, 253 ) }
};
/*--------------------------------------------------------------------------------------------------------------------*/

HueAmbianceLight::HueAmbianceLight( int iID, QObject * pParent ) : HueLight( iID, pParent ), m_iColorTemperature( 0 )
{
    // Nothing else to do.
}
/*--------------------------------------------------------------------------------------------------------------------*/

int HueAmbianceLight::minColorTemperature() const
{
    return MIN_CAPABLE_CT_K;
}
/*--------------------------------------------------------------------------------------------------------------------*/

int HueAmbianceLight::maxColorTemperature() const
{
    return MAX_CAPABLE_CT_K;
}
/*--------------------------------------------------------------------------------------------------------------------*/

QColor HueAmbianceLight::ambientColor() const
{
    // Take the nearest reference point.
    int iRemainder = m_iColorTemperature % 100;
    int iReference = m_iColorTemperature - iRemainder;
    if ( 50 <= iRemainder )
    {
        iReference += 100;
    }

    return COLOR_TEMPERATURE_MAP.value( iReference );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueAmbianceLight::commandColorTemperature( int iColorTemperature )
{
    if ( ( MIN_CAPABLE_CT_K <= iColorTemperature ) && ( MAX_CAPABLE_CT_K >= iColorTemperature ) )
    {
        // Scale the friendly color temperature into the capable range, accounting for it being reversed.
        iColorTemperature = ( ( 1.0
                                - ( static_cast<double>( iColorTemperature ) - MIN_CAPABLE_CT_K )
                                      / ( MAX_CAPABLE_CT_K - MIN_CAPABLE_CT_K ) )
                              * ( MIN_CAPABLE_CT - MAX_CAPABLE_CT ) )
                            + MAX_CAPABLE_CT;
        VCHub::instance()->hue()->commandDeviceState( m_iID, QJsonObject { { "ct", iColorTemperature } } );
    }
    else
    {
        qDebug() << "Ignoring request to set invalid color temperature for light with ID: " << m_iID;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueAmbianceLight::handleStateData( const QJsonObject & State )
{
    // Call the parent.
    HueLight::handleStateData( State );

    if ( State.contains( "ct" ) )
    {
        // Scale the reported value in the friendly display range, accounting for it being reversed.
        int iColorTemperature =
            ( ( 1.0 - ( ( State.value( "ct" ).toDouble() - MAX_CAPABLE_CT ) / ( MIN_CAPABLE_CT - MAX_CAPABLE_CT ) ) )
              * ( MAX_CAPABLE_CT_K - MIN_CAPABLE_CT_K ) )
            + MIN_CAPABLE_CT_K;  // Account for the reversed range
        if ( m_iColorTemperature != iColorTemperature )
        {
            m_iColorTemperature = iColorTemperature;
            emit colorTemperatureChanged();
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
