#include <QJsonArray>
#include <QtMath>

#include "huecolorlight.h"
#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

HueColorLight::HueColorLight( int id, QObject * parent ) :
    HueAmbianceLight( id, parent )
{
    // Nothing else to do.
}
/*--------------------------------------------------------------------------------------------------------------------*/

QColor HueColorLight::xyToColor( const double xIn, const double yIn )
{
    // Calculate XYZ values.
    double y = 1.0;  // Always use full brightness for visual purposes.
    double x = ( y / yIn ) * xIn;
    double z = ( y / yIn ) * ( 1.0f - xIn - yIn );

    // Convert to RGB using Wide RGB D65 conversion.
    double r = ( x * 1.656492f ) - ( y * 0.354851 ) - ( z * 0.255038 );
    double g = ( -x * 0.707196 ) + ( y * 1.655397 ) + ( z * 0.036152 );
    double b = ( x * 0.051713 ) - ( y * 0.121364 ) + ( z * 1.011530 );

    // Apply reverse gamma correction.
    r = ( r <= 0.0031308 ) ? ( 12.92 * r ) : ( ( 1.0 + 0.055 ) * qPow( r, ( 1.0 / 2.4 ) ) - 0.055 );
    g = ( g <= 0.0031308 ) ? ( 12.92 * g ) : ( ( 1.0 + 0.055 ) * qPow( g, ( 1.0 / 2.4 ) ) - 0.055 );
    b = ( b <= 0.0031308 ) ? ( 12.92 * b ) : ( ( 1.0 + 0.055 ) * qPow( b, ( 1.0 / 2.4 ) ) - 0.055 );

    // Bring all negative components to 0.
    r = qMax( r, 0.0 );
    g = qMax( g, 0.0 );
    b = qMax( b, 0.0 );

    // Scale the components to be in range if necessary.
    double maxComponent = qMax( r, qMax( g, b ) );
    if ( 1.0 < maxComponent )
    {
        r = r / maxComponent;
        g = g / maxComponent;
        b = b / maxComponent;
    }

    // Construct the color object from the RGB components.
    return QColor::fromRgbF( r, g, b );
}
/*--------------------------------------------------------------------------------------------------------------------*/

QColor HueColorLight::hueToColor( const int hue )
{
    return QColor::fromHsv( hue, 255, 255 );  // Assume maximum saturation and brightness
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueColorLight::commandColor( const QColor & color )
{
    if ( color.isValid() )
    {
        double rIn = color.redF();
        double gIn = color.greenF();
        double bIn = color.blueF();

        // Apply gamma correction.
        double r = ( rIn > 0.04045 ) ? qPow( ( ( rIn + 0.055 ) / ( 1.0 + 0.055 ) ), 2.4 ) : ( rIn / 12.92 );
        double g = ( gIn > 0.04045 ) ? qPow( ( ( gIn + 0.055 ) / ( 1.0 + 0.055 ) ), 2.4 ) : ( gIn / 12.92 );
        double b = ( bIn > 0.04045 ) ? qPow( ( ( bIn + 0.055 ) / ( 1.0 + 0.055 ) ), 2.4 ) : ( gIn / 12.92 );

        // Convert to XYZ using Wide RGB D65 conversion.
        double x = ( r * 0.664511 ) + ( g * 0.154324 ) + ( b * 0.162028 );
        double y = ( r * 0.283881 ) + ( g * 0.668433 ) + ( b * 0.047685 );
        double z = ( r * 0.000088 ) + ( g * 0.072310 ) + ( b * 0.986039 );

        // Calculate XY values.
        double cx = x / ( x + y + z );
        double cy = y / ( x + y + z );

        // Package the values and send them out.
        commandColor( cx, cy );
    }
    else
    {
        qDebug() << "Ignoring request to set invalid color for light with ID: " << id_;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueColorLight::commandColor( const int hue )
{
    if ( ( 0 <= hue ) && ( 359 >= hue ) )
    {
        commandColor( hueToColor( hue ) );
    }
    else
    {
        qDebug() << "Ignoring request to set invalid hue for light with ID: " << id_;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueColorLight::commandColor( const double x, const double y )
{
    if ( !qIsNaN( x ) && !qIsNaN( y ) )
    {
        // If the light is not on, turn it on or else the command will fail.
        if ( !isOn_ )
        {
            commandPower( true );
        }

        // BDP: Updated color temperature may not be reported back for several seconds.
        VCHub::instance()->hue()->commandDeviceState( id_, QJsonObject { { "xy", QJsonArray { x, y } } } );
    }
    else
    {
        qDebug() << "Ignoring request to set invalid XY for light with ID: " << id_;
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void HueColorLight::handleStateData( const QJsonObject & state )
{
    // Call the parent.
    HueAmbianceLight::handleStateData( state );

    // BDP: Use XY instead of HSV because it is hardware-independent. Even though some accuracy may be lost in these
    //      calculations, the resulting colors (in and out) should be visually indistinguishable in most cases as
    //      opposed to HSV, where colors displayed and commanded may appear slightly different depending on the light.
    if ( state.contains( "xy" ) )
    {
        QJsonArray xy = state.value( "xy" ).toArray();
        if ( 2 == xy.size() )
        {
            QColor color = xyToColor( xy.at( 0 ).toDouble(), xy.at( 1 ).toDouble() );
            if ( color_ != color )
            {
                color_ = color;
                emit colorChanged();
            }
        }
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
