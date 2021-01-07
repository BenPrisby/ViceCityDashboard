#include "vcinsults.h"
#include "networkinterface.h"
#include "vchub.h"
/*--------------------------------------------------------------------------------------------------------------------*/

VCInsults::VCInsults( const QString & Name, QObject * pParent ) :
    VCPlugin( Name, pParent ),
    m_RequestURL( QString( "https://evilinsult.com/generate_insult.php?lang=%1" ).arg( QLocale::system().bcp47Name() ) )
{
    setUpdateInterval( 60 * 1000 );

    // Handle network responses.
    connect( NetworkInterface::instance(), &NetworkInterface::replyReceived, this, &VCInsults::handleNetworkReply );

    refresh();
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCInsults::refresh()
{
    NetworkInterface::instance()->sendRequest( m_RequestURL, this );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCInsults::handleNetworkReply( const int iStatusCode, QObject * const pSender, const QByteArray & Body )
{
    if ( this == pSender )
    {
        if ( 200 == iStatusCode )
        {
            QString Insult = fromHtmlEscaped( QString::fromUtf8( Body ) );
            if ( !Insult.isEmpty() )
            {
                m_Insult = Insult;
                emit insultChanged();
            }
            else
            {
                qDebug() << "No insult received in reply";
            }
        }
        else
        {
            qDebug() << "Ignoring bad reply when requesting insult";
        }
    }
    else
    {
        // Not for us, ignore.
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

QString VCInsults::fromHtmlEscaped( QString Text )
{
    Text.replace( "&quot;", QChar( '"' ) );
    Text.replace( "&apos;", QChar( '\'' ) );
    Text.replace( "&amp;", QChar( '&' ) );
    Text.replace( "&lt;", QChar( '<' ) );
    Text.replace( "&gt;", QChar( '>' ) );
    return Text;
}
/*--------------------------------------------------------------------------------------------------------------------*/
