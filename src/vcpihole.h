#ifndef VCPIHOLE_H_
#define VCPIHOLE_H_

#include <QHostInfo>
#include <QUrl>

#include "vcplugin.h"

class VCPiHole final : public VCPlugin
{
    Q_OBJECT
    Q_PROPERTY( QString serverHostname      READ serverHostname      MEMBER m_ServerHostname  NOTIFY serverHostnameChanged )
    Q_PROPERTY( QString serverIPAddress     READ serverIPAddress                              NOTIFY serverIPAddressChanged )
    Q_PROPERTY( bool isEnabled              READ isEnabled                                    NOTIFY isEnabledChanged )
    Q_PROPERTY( int totalQueries            READ totalQueries                                 NOTIFY totalQueriesChanged )
    Q_PROPERTY( int blockedQueries          READ blockedQueries                               NOTIFY blockedQueriesChanged )
    Q_PROPERTY( double percentBlocked       READ percentBlocked                               NOTIFY percentBlockedChanged )
    Q_PROPERTY( int blockedDomains          READ blockedDomains                               NOTIFY blockedDomainsChanged )
    Q_PROPERTY( QVariantMap historicalData  READ historicalData                               NOTIFY historicalDataChanged )

public:
    explicit VCPiHole( const QString & Name, QObject * pParent = nullptr );

    const QString & serverHostname() const { return m_ServerHostname; }
    const QString & serverIPAddress() const { return m_ServerIPAddress; }
    bool isEnabled() const { return m_bIsEnabled; }
    int totalQueries() const { return m_iTotalQueries; }
    int blockedQueries() const { return m_iBlockedQueries; }
    double percentBlocked() const { return m_dPercentBlocked; }
    int blockedDomains() const { return m_iBlockedDomains; }
    const QVariantMap & historicalData() const { return m_HistoricalData; }

signals:
    void serverHostnameChanged();
    void serverIPAddressChanged();
    void isEnabledChanged();
    void totalQueriesChanged();
    void blockedQueriesChanged();
    void percentBlockedChanged();
    void blockedDomainsChanged();
    void historicalDataChanged();

public slots:
    void refresh() override;
    void refreshHistoricalData();

private slots:
    void handleHostLookup( const QHostInfo & Host );
    void handleNetworkReply( int iStatusCode, QObject * pSender, const QJsonDocument & Body );

private:
    QString m_ServerHostname;
    QString m_ServerIPAddress;
    bool m_bIsEnabled;
    int m_iTotalQueries;
    int m_iBlockedQueries;
    double m_dPercentBlocked;
    int m_iBlockedDomains;
    QVariantMap m_HistoricalData;

    QUrl m_SummaryDestination;
    QUrl m_HistoricalDataDestination;

    Q_DISABLE_COPY_MOVE( VCPiHole )
};

#endif  // VCPIHOLE_H_
