#include <QDebug>

#include "vcplugin.h"
/*--------------------------------------------------------------------------------------------------------------------*/

VCPlugin::VCPlugin( const QString & Name, QObject * pParent ) :
    QObject( pParent ), m_Name( Name ), m_iUpdateInterval( 10 * 1000 )
{
    if ( !m_Name.isEmpty() )
    {
        setObjectName( m_Name );
        qDebug() << "Initializing plugin: " << m_Name;

        // Configure the update timer for periodically refreshing any attached data.
        m_UpdateTimer.setInterval( m_iUpdateInterval );
        m_UpdateTimer.setSingleShot( false );
        connect( &m_UpdateTimer, &QTimer::timeout, this, &VCPlugin::refresh );
        m_UpdateTimer.start();
    }
    else
    {
        qFatal( "Missing name for VCPlugin" );
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCPlugin::setUpdateInterval( const int iValue )
{
    if ( m_iUpdateInterval != iValue )
    {
        m_iUpdateInterval = iValue;
        m_UpdateTimer.setInterval( m_iUpdateInterval );
        emit updateIntervalChanged();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
