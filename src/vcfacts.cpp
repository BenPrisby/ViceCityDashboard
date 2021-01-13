#include "vcfacts.h"
#include "networkinterface.h"
#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

VCFacts::VCFacts( const QString & Name, QObject * pParent ) :
    VCPlugin( Name, pParent ),
    m_RequestURL( QString( "https://uselessfacts.jsph.pl/random.json?language=%1" ).arg( QLocale::system().bcp47Name() ) )
{
    setUpdateInterval( 60 * 1000 );

    // Handle network responses.
    connect( NetworkInterface::instance(), &NetworkInterface::jsonReplyReceived, this, &VCFacts::handleNetworkReply );

    refresh();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCFacts::refresh()
{
    NetworkInterface::instance()->sendJSONRequest( m_RequestURL, this );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCFacts::handleNetworkReply( const int iStatusCode, QObject * const pSender, const QJsonDocument & Body )
{
    if ( this == pSender )
    {
        if ( 200 == iStatusCode )
        {
            if ( Body.isObject() )
            {
                QJsonObject ResponseObject = Body.object();
                if ( ResponseObject.contains( "text" ) )
                {
                    QString Fact = ResponseObject.value( "text" ).toString().simplified()
                            .replace( QChar( '`' ), QChar( '\'' ) );
                    if ( !Fact.isEmpty() )
                    {
                        m_Fact = Fact;
                        emit factChanged();
                    }
                    else
                    {
                        qDebug() << "No fact received in reply";
                    }
                }
            }
        }
        else
        {
            qDebug() << "Ignoring bad reply when requesting fact";
        }
    }
    else
    {
        // Not for us, ignore.
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
