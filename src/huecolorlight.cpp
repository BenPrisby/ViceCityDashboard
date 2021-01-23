#include <QJsonArray>
#include <QtMath>

#include "huecolorlight.h"
#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

HueColorLight::HueColorLight( int iID, QObject * pParent ) :
    HueAmbianceLight( iID, pParent )
{
    // Nothing else to do.
}
/*--------------------------------------------------------------------------------------------------------------------*/

QColor HueColorLight::xyToColor( const double dXIn, const double dYIn )
{
    // Calculate XYZ values.
    double dY = 1.0;  // Always use full brightness for visual purposes.
    double dX = ( dY / dYIn ) * dXIn;
    double dZ = ( dY / dYIn ) * ( 1.0f - dXIn - dYIn );

    // Convert to RGB using Wide RGB D65 conversion.
    double dR = ( dX * 1.656492f ) - ( dY * 0.354851 ) - ( dZ * 0.255038 );
    double dG = ( -dX * 0.707196 ) + ( dY * 1.655397 ) + ( dZ * 0.036152 );
    double dB = ( dX * 0.051713 ) - ( dY * 0.121364 ) + ( dZ * 1.011530 );

    // Apply reverse gamma correction.
    dR = ( dR <= 0.0031308 ) ? ( 12.92 * dR ) : ( ( 1.0 + 0.055 ) * qPow( dR, ( 1.0 / 2.4 ) ) - 0.055 );
    dG = ( dG <= 0.0031308 ) ? ( 12.92 * dG ) : ( ( 1.0 + 0.055 ) * qPow( dG, ( 1.0 / 2.4 ) ) - 0.055 );
    dB = ( dB <= 0.0031308 ) ? ( 12.92 * dB ) : ( ( 1.0 + 0.055 ) * qPow( dB, ( 1.0 / 2.4 ) ) - 0.055 );

    // Bring all negative components to 0.
    dR = qMax( dR, 0.0 );
    dG = qMax( dG, 0.0 );
    dB = qMax( dB, 0.0 );

    // Scale the components to be in range if necessary.
    double dMaxComponent = qMax( dR, qMax( dG, dB ) );
    if ( 1.0 < dMaxComponent )
    {
        dR = dR / dMaxComponent;
        dG = dG / dMaxComponent;
        dB = dB / dMaxComponent;
    }

    // Construct the color object from the RGB components.
    return QColor::fromRgbF( dR, dG, dB );
}
/*--------------------------------------------------------------------------------------------------------------------*/

QColor HueColorLight::hueToColor( const int iHue )
{
    return QColor::fromHsv( iHue, 255, 255 );  // Assume maximum saturation and brightness
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueColorLight::commandColor( const QColor & Color )
{
    if ( Color.isValid() )
    {
        double dRIn = Color.redF();
        double dGIn = Color.greenF();
        double dBIn = Color.blueF();

        // Apply gamma correction.
        double dR = ( dRIn > 0.04045 ) ? qPow( ( ( dRIn + 0.055 ) / ( 1.0 + 0.055 ) ), 2.4 ) : ( dRIn / 12.92 );
        double dG = ( dGIn > 0.04045 ) ? qPow( ( ( dGIn + 0.055 ) / ( 1.0 + 0.055 ) ), 2.4 ) : ( dGIn / 12.92 );
        double dB = ( dBIn > 0.04045 ) ? qPow( ( ( dBIn + 0.055 ) / ( 1.0 + 0.055 ) ), 2.4 ) : ( dGIn / 12.92 );

        // Convert to XYZ using Wide RGB D65 conversion.
        double dX = ( dR * 0.664511 ) + ( dG * 0.154324 ) + ( dB * 0.162028 );
        double dY = ( dR * 0.283881 ) + ( dG * 0.668433 ) + ( dB * 0.047685 );
        double dZ = ( dR * 0.000088 ) + ( dG * 0.072310 ) + ( dB * 0.986039 );

        // Calculate XY values.
        double dCX = dX / ( dX + dY + dZ );
        double dCY = dY / ( dX + dY + dZ );

        // Package the values and send them out.
        commandColor( dCX, dCY );
    }
    else
    {
        qDebug() << "Ignoring request to set invalid color for light with ID: " << m_iID;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueColorLight::commandColor( const int iHue )
{
    if ( ( 0 <= iHue ) && ( 359 >= iHue ) )
    {
        commandColor( hueToColor( iHue ) );
    }
    else
    {
        qDebug() << "Ignoring request to set invalid hue for light with ID: " << m_iID;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueColorLight::commandColor( const double dX, const double dY )
{
    if ( ( !qIsNaN( dX ) ) && ( !qIsNaN( dY ) ) )
    {
        // If the light is not on, turn it on or else the command will fail.
        if ( !m_bIsOn )
        {
            commandPower( true );
        }

        // BDP: Updated color temperature may not be reported back for several seconds.
        VCHub::instance()->hue()->commandDeviceState( m_iID, QJsonObject { { "xy", QJsonArray { dX, dY } } } );
    }
    else
    {
        qDebug() << "Ignoring request to set invalid XY for light with ID: " << m_iID;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueColorLight::handleStateData( const QJsonObject & State )
{
    // Call the parent.
    HueAmbianceLight::handleStateData( State );

    // BDP: Use XY instead of HSV because it is hardware-independent. Even though some accuracy may be lost in these
    //      calculations, the resulting colors (in and out) should be visually indistinguishable in most cases as
    //      opposed to HSV, where colors displayed and commanded may appear slightly different depending on the light.
    if ( State.contains( "xy" ) )
    {
        QJsonArray XY = State.value( "xy" ).toArray();
        if ( 2 == XY.size() )
        {
            QColor Color = xyToColor( XY.at( 0 ).toDouble(), XY.at( 1 ).toDouble() );
            if ( m_Color != Color )
            {
                m_Color = Color;
                emit colorChanged();
            }
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
