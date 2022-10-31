#ifndef VCPIHOLE_H_
#define VCPIHOLE_H_

#include <QHostInfo>
#include <QUrl>

#include "vcplugin.h"

class VCPiHole final : public VCPlugin {
    Q_OBJECT

    // clang-format off
    Q_PROPERTY(QString serverHostname      MEMBER serverHostname_  READ serverHostname   NOTIFY serverHostnameChanged)
    Q_PROPERTY(QString serverIPAddress                             READ serverIPAddress  NOTIFY serverIPAddressChanged)
    Q_PROPERTY(bool isEnabled                                      READ isEnabled        NOTIFY isEnabledChanged)
    Q_PROPERTY(int totalQueries                                    READ totalQueries     NOTIFY totalQueriesChanged)
    Q_PROPERTY(int blockedQueries                                  READ blockedQueries   NOTIFY blockedQueriesChanged)
    Q_PROPERTY(double percentBlocked                               READ percentBlocked   NOTIFY percentBlockedChanged)
    Q_PROPERTY(int blockedDomains                                  READ blockedDomains   NOTIFY blockedDomainsChanged)
    Q_PROPERTY(QVariantMap historicalData                          READ historicalData   NOTIFY historicalDataChanged)
    // clang-format on

 public:
    explicit VCPiHole(const QString& name, QObject* parent = nullptr);

    const QString& serverHostname() const { return serverHostname_; }
    const QString& serverIPAddress() const { return serverIPAddress_; }
    bool isEnabled() const { return isEnabled_; }
    int totalQueries() const { return totalQueries_; }
    int blockedQueries() const { return blockedQueries_; }
    double percentBlocked() const { return percentBlocked_; }
    int blockedDomains() const { return blockedDomains_; }
    const QVariantMap& historicalData() const { return historicalData_; }

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
    void handleHostLookup(const QHostInfo& host);
    void handleNetworkReply(int iStatusCode, QObject* sender, const QJsonDocument& body);

 private:
    QString serverHostname_;
    QString serverIPAddress_;
    bool isEnabled_;
    int totalQueries_;
    int blockedQueries_;
    double percentBlocked_;
    int blockedDomains_;
    QVariantMap historicalData_;

    QUrl summaryDestination_;
    QUrl historicalDataDestination_;

    Q_DISABLE_COPY_MOVE(VCPiHole)
};

#endif  // VCPIHOLE_H_
