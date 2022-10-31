#include "vcpihole.h"

#include <QJsonObject>

#include "networkinterface.h"
/*--------------------------------------------------------------------------------------------------------------------*/

VCPiHole::VCPiHole(const QString& name, QObject* parent)
    : VCPlugin(name, parent),
      isEnabled_(false),
      totalQueries_(0),
      blockedQueries_(0),
      percentBlocked_(qQNaN()),
      blockedDomains_(0) {
    // Don't start refreshing until the Pi Hole server has been found.
    updateTimer_.stop();
    setUpdateInterval(1000);

    // Handle network responses.
    connect(NetworkInterface::instance(), &NetworkInterface::jsonReplyReceived, this, &VCPiHole::handleNetworkReply);

    // Look for the Pi Hole server when the hostname is populated.
    connect(this, &VCPiHole::serverHostnameChanged, this, [this] {
        if (!serverHostname_.isEmpty()) {
            (void)QHostInfo::lookupHost(serverHostname_, this, &VCPiHole::handleHostLookup);
        }
    });
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCPiHole::refresh() {
    NetworkInterface::instance()->sendJSONRequest(summaryDestination_, this);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCPiHole::refreshHistoricalData() {
    NetworkInterface::instance()->sendJSONRequest(historicalDataDestination_, this);
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCPiHole::handleHostLookup(const QHostInfo& host) {
    if (QHostInfo::NoError == host.error()) {
        const QList<QHostAddress> addresses = host.addresses();
        for (const auto& address : addresses) {
            if (QAbstractSocket::IPv4Protocol == address.protocol()) {
                serverIPAddress_ = address.toString();
                qDebug() << "Pi Hole server found at IP address: " << serverIPAddress_;

                // Update the destination URL and start refreshing information.
                QString baseURL = QString("http://%1/admin/api.php").arg(serverIPAddress_);
                summaryDestination_ = QUrl(QString("%1?%2").arg(baseURL, "summaryRaw"));
                historicalDataDestination_ = QUrl(QString("%1?%2").arg(baseURL, "overTimeData10mins"));
                updateTimer_.start();
                refresh();
                refreshHistoricalData();
                break;
            } else {
                // IPv6 is the protocol of the future and always will be.
            }
        }
    } else {
        qDebug() << "Failed to find Pi Hole server on the local network with error: " << host.errorString();
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/

void VCPiHole::handleNetworkReply(int statusCode, QObject* sender, const QJsonDocument& body) {
    if (this == sender) {
        if (200 == statusCode) {
            if (body.isObject()) {
                QJsonObject responseObject = body.object();
                if (responseObject.contains("status")) {
                    bool enabled = ("enabled" == responseObject.value("status").toString());
                    if (isEnabled_ != enabled) {
                        isEnabled_ = enabled;
                        emit isEnabledChanged();
                    }
                }
                if (responseObject.contains("dns_queries_today")) {
                    int totalQueries = responseObject.value("dns_queries_today").toInt();
                    if (totalQueries_ != totalQueries) {
                        totalQueries_ = totalQueries;
                        emit totalQueriesChanged();
                    }
                }
                if (responseObject.contains("ads_blocked_today")) {
                    int blockedQueries = responseObject.value("ads_blocked_today").toInt();
                    if (blockedQueries_ != blockedQueries) {
                        blockedQueries_ = blockedQueries;
                        emit blockedQueriesChanged();
                    }
                }
                if (responseObject.contains("ads_percentage_today")) {
                    // BDP: We could easily calculate this, but since it's included in the response just use it.
                    double percentBlocked = responseObject.value("ads_percentage_today").toDouble();
                    if (percentBlocked_ != percentBlocked) {
                        percentBlocked_ = percentBlocked;
                        emit percentBlockedChanged();
                    }
                }
                if (responseObject.contains("domains_being_blocked")) {
                    int blockedDomains = responseObject.value("domains_being_blocked").toInt();
                    if (blockedDomains_ != blockedDomains) {
                        blockedDomains_ = blockedDomains;
                        emit blockedDomainsChanged();
                    }
                }
                if (responseObject.contains("domains_over_time") && responseObject.contains("ads_over_time")) {
                    QJsonObject domainsOverTime = responseObject.value("domains_over_time").toObject();
                    QJsonObject adsOverTime = responseObject.value("ads_over_time").toObject();
                    if (!domainsOverTime.isEmpty() && (domainsOverTime.size() == adsOverTime.size())) {
                        QVariantList timestamps;
                        QVariantList totalQueries;
                        QVariantList blockedQueries;
                        QVariantList allowedQueries;
                        QVariantList blockPercentages;
                        int maxTotalQueries = 0;
                        double minBlockPercentage = 100.0;
                        double maxBlockPercentage = 0.0;

                        const QStringList keys = domainsOverTime.keys();
                        for (const auto& key : keys) {
                            timestamps.append(key.toInt());
                        }
                        std::sort(timestamps.begin(), timestamps.end());  // Arrange chronologically
                        for (const auto& timestamp : timestamps) {
                            int total = domainsOverTime.value(timestamp.toString()).toInt();
                            int blocked = adsOverTime.value(timestamp.toString()).toInt();
                            int allowed = total - blocked;
                            double blockPercentage = (static_cast<double>(blocked) / total) * 100.0;

                            totalQueries.append(total);
                            blockedQueries.append(blocked);
                            allowedQueries.append(allowed);
                            blockPercentages.append(blockPercentage);

                            // Keep track of minimums and maximums in the sets.
                            if (maxTotalQueries < total) {
                                maxTotalQueries = total;
                            }
                            if (minBlockPercentage > blockPercentage) {
                                minBlockPercentage = blockPercentage;
                            }
                            if (maxBlockPercentage < blockPercentage) {
                                maxBlockPercentage = blockPercentage;
                            }
                        }

                        // Expose as a model for graphing in QML, with lists of data points and some statistics.
                        historicalData_ = QVariantMap{{"timestamps", timestamps},
                                                      {"totalQueries", totalQueries},
                                                      {"maxTotalQueries", maxTotalQueries},
                                                      {"blockedQueries", blockedQueries},
                                                      {"allowedQueries", allowedQueries},
                                                      {"blockPercentages", blockPercentages},
                                                      {"minBlockPercentage", minBlockPercentage},
                                                      {"maxBlockPercentage", maxBlockPercentage}};
                        emit historicalDataChanged();
                    }
                }
            } else {
                qDebug() << "Failed to parse response from Pi Hole server";
            }
        } else {
            qDebug() << "Ignoring unsuccessful reply from Pi Hole server with status code: " << statusCode;
        }
    } else {
        // Not for us, ignore.
    }
}
/*--------------------------------------------------------------------------------------------------------------------*/
