#include <QJsonObject>

#include "vcpihole.h"
#include "networkinterface.h"
/*--------------------------------------------------------------------------------------------------------------------*/

VCPiHole::VCPiHole( const QString & Name, QObject * pParent ) :
    VCPlugin( Name, pParent ),
    m_bIsEnabled( false ),
    m_iTotalQueries( 0 ),
    m_iBlockedQueries( 0 ),
    m_dPercentBlocked( qQNaN() ),
    m_iBlockedDomains( 0 )
{
    // Don't start refreshing until the Pi Hole server has been found.
    m_UpdateTimer.stop();
    setUpdateInterval( 1000 );

    // Handle network responses.
    connect( NetworkInterface::instance(), &NetworkInterface::jsonReplyReceived, this, &VCPiHole::handleNetworkReply );

    // Look for the Pi Hole server when the hostname is populated.
    connect( this, &VCPiHole::serverHostnameChanged, this, [=] {
        if ( !m_ServerHostname.isEmpty() )
        {
            ( void )QHostInfo::lookupHost( m_ServerHostname, this, &VCPiHole::handleHostLookup );
        }
    } );

    // Set up a less frequent timer for fetching historical data.
    m_HistoricalDataRefreshTimer.setInterval( 60 * 1000 );
    m_HistoricalDataRefreshTimer.setSingleShot( false );
    connect( &m_HistoricalDataRefreshTimer, &QTimer::timeout, this, &VCPiHole::refreshHistoricalData );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCPiHole::refresh()
{
    NetworkInterface::instance()->sendJSONRequest( m_SummaryDestination, this );
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCPiHole::handleNetworkReply( int iStatusCode, QObject * pSender, const QJsonDocument & Body )
{
    if ( this == pSender )
    {
        if ( 200 == iStatusCode )
        {
            if ( Body.isObject() )
            {
                QJsonObject ResponseObject = Body.object();
                if ( ResponseObject.contains( "status" ) )
                {
                    bool bEnabled = ( "enabled" == ResponseObject.value( "status" ).toString() );
                    if ( m_bIsEnabled != bEnabled )
                    {
                        m_bIsEnabled = bEnabled;
                        emit isEnabledChanged();
                    }
                }
                if ( ResponseObject.contains( "dns_queries_today" ) )
                {
                    int iTotalQueries = ResponseObject.value( "dns_queries_today" ).toInt();
                    if ( m_iTotalQueries != iTotalQueries )
                    {
                        m_iTotalQueries = iTotalQueries;
                        emit totalQueriesChanged();
                    }
                }
                if ( ResponseObject.contains( "ads_blocked_today" ) )
                {
                    int iBlockedQueries = ResponseObject.value( "ads_blocked_today" ).toInt();
                    if ( m_iBlockedQueries != iBlockedQueries )
                    {
                        m_iBlockedQueries = iBlockedQueries;
                        emit blockedQueriesChanged();
                    }
                }
                if ( ResponseObject.contains( "ads_percentage_today" ) )
                {
                    // BDP: We could easily calculate this, but since it's included in the response just use it.
                    double dPercentBlocked = ResponseObject.value( "ads_percentage_today" ).toDouble();
                    if ( m_dPercentBlocked != dPercentBlocked )
                    {
                        m_dPercentBlocked = dPercentBlocked;
                        emit percentBlockedChanged();
                    }
                }
                if ( ResponseObject.contains( "domains_being_blocked" ) )
                {
                    int iBlockedDomains = ResponseObject.value( "domains_being_blocked" ).toInt();
                    if ( m_iBlockedDomains != iBlockedDomains )
                    {
                        m_iBlockedDomains = iBlockedDomains;
                        emit blockedDomainsChanged();
                    }
                }
                if ( ResponseObject.contains( "domains_over_time" ) && ResponseObject.contains( "ads_over_time" ) )
                {
                    QJsonObject DomainsOverTime = ResponseObject.value( "domains_over_time" ).toObject();
                    QJsonObject AdsOverTime = ResponseObject.value( "ads_over_time" ).toObject();
                    if ( ( !DomainsOverTime.isEmpty() ) && ( DomainsOverTime.size() == AdsOverTime.size() ) )
                    {
                        QVariantList Timestamps;
                        QVariantList TotalQueries;
                        QVariantList BlockedQueries;
                        QVariantList AllowedQueries;
                        QVariantList BlockPercentages;
                        int iMaxTotalQueries = 0;
                        double dMinBlockPercentage = 100.0;
                        double dMaxBlockPercentage = 0.0;

                        const QStringList Keys = DomainsOverTime.keys();
                        for ( const auto & Key : Keys )
                        {
                            Timestamps.append( Key.toInt() );
                        }
                        std::sort( Timestamps.begin(), Timestamps.end() );  // Arrange chronologically
                        for ( const auto & Timestamp : Timestamps )
                        {
                            int iTotal = DomainsOverTime.value( Timestamp.toString() ).toInt();
                            int iBlocked = AdsOverTime.value( Timestamp.toString() ).toInt();
                            int iAllowed = iTotal - iBlocked;
                            double dBlockPercentage = ( static_cast<double>( iBlocked ) / iTotal ) * 100.0;

                            TotalQueries.append( iTotal );
                            BlockedQueries.append( iBlocked );
                            AllowedQueries.append( iAllowed );
                            BlockPercentages.append( dBlockPercentage );

                            // Keep track of minimums and maximums in the sets.
                            if ( iMaxTotalQueries < iTotal )
                            {
                                iMaxTotalQueries = iTotal;
                            }
                            if ( dMinBlockPercentage > dBlockPercentage )
                            {
                                dMinBlockPercentage = dBlockPercentage;
                            }
                            if ( dMaxBlockPercentage < dBlockPercentage )
                            {
                                dMaxBlockPercentage = dBlockPercentage;
                            }
                        }

                        // Expose as a model for graphing in QML, with lists of data points and some statistics.
                        m_HistoricalData = QVariantMap { { "timestamps", Timestamps },
                                                         { "totalQueries", TotalQueries },
                                                         { "maxTotalQueries", iMaxTotalQueries },
                                                         { "blockedQueries", BlockedQueries },
                                                         { "allowedQueries", AllowedQueries },
                                                         { "blockPercentages", BlockPercentages },
                                                         { "minBlockPercentage", dMinBlockPercentage },
                                                         { "maxBlockPercentage", dMaxBlockPercentage } };
                        emit historicalDataChanged();
                    }
                }
            }
            else
            {
                qDebug() << "Failed to parse response from Pi Hole server";
            }
        }
        else
        {
            qDebug() << "Ignoring unsuccessful reply from Pi Hole server with status code: " << iStatusCode;
        }
    }
    else
    {
        // Not for us, ignore.
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCPiHole::handleHostLookup( const QHostInfo & Host )
{
    if ( QHostInfo::NoError == Host.error() )
    {
        const QList<QHostAddress> Addresses = Host.addresses();
        for ( const auto & Address : Addresses )
        {
            if ( QAbstractSocket::IPv4Protocol == Address.protocol() )
            {
                m_ServerIPAddress = Address.toString();
                qDebug() << "Pi Hole server found at IP address: " << m_ServerIPAddress;

                // Update the destination URL and start refreshing information.
                QString BaseURL = QString( "http://%1/admin/api.php" ).arg( m_ServerIPAddress );
                m_SummaryDestination = QUrl( QString( "%1?%2" ).arg( BaseURL, "summaryRaw" ) );
                m_HistoricalDataDestination = QUrl( QString( "%1?%2" ).arg( BaseURL, "overTimeData10mins" ) );
                m_UpdateTimer.start();
                m_HistoricalDataRefreshTimer.start();
                refresh();
                refreshHistoricalData();
                break;
            }
            else
            {
                // IPv6 is the protocol of the future and always will be.
            }
        }
    }
    else
    {
        qDebug() << "Failed to find Pi Hole server on the local network with error: " << Host.errorString();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCPiHole::refreshHistoricalData()
{
    NetworkInterface::instance()->sendJSONRequest( m_HistoricalDataDestination, this );
}
/*--------------------------------------------------------------------------------------------------------------------*/
