#include "huelight.h"
#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

static constexpr int MIN_CAPABLE_BRIGHTNESS = 1;
static constexpr int MAX_CAPABLE_BRIGHTNESS = 254;
/*--------------------------------------------------------------------------------------------------------------------*/

HueLight::HueLight( int iID, QObject * pParent ) : HueDevice( iID, pParent )
{
    // Nothing else to do.
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueLight::commandBrightness( const double dBrightness )
{
    if ( ( !qIsNaN( dBrightness ) ) && ( 0.0 <= dBrightness ) && ( 100.0 >= dBrightness ) )
    {
        // If the light is not on, turn it on or else the command will fail.
        if ( !m_bIsOn )
        {
            commandPower( true );
        }

        // Scale from a percentage into the capable range of the light.
        int iBrightness = qRound( ( ( dBrightness / 100.0 ) * ( MAX_CAPABLE_BRIGHTNESS - MIN_CAPABLE_BRIGHTNESS ) )
                                  + MIN_CAPABLE_BRIGHTNESS );
        VCHub::instance()->hue()->commandDeviceState( m_iID, QJsonObject { { "bri", iBrightness } } );
    }
    else
    {
        qDebug() << "Ignoring request to set invalid brightness for light with ID: " << m_iID;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueLight::handleStateData( const QJsonObject & State )
{
    // Call the parent.
    HueDevice::handleStateData( State );

    if ( State.contains( "bri" ) )
    {
        // Present the brightness as a percentage.
        double dBrightness = ( ( State.value( "bri" ).toDouble() - MIN_CAPABLE_BRIGHTNESS )
                               / ( MAX_CAPABLE_BRIGHTNESS - MIN_CAPABLE_BRIGHTNESS ) )
                             * 100.0;
        if ( m_dBrightness != dBrightness )
        {
            m_dBrightness = dBrightness;
            emit brightnessChanged();
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
