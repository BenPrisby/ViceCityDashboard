#ifndef VCHUE_H_
#define VCHUE_H_

#include "huedevice.h"
#include "huelight.h"
#include "hueambiancelight.h"
#include "huecolorlight.h"
#include "vcplugin.h"

class VCHue : public VCPlugin
{
    Q_OBJECT
    Q_PROPERTY( QList<HueDevice *> devices  READ devices             NOTIFY devicesChanged )
    Q_PROPERTY( int onDevicesCount          READ onDevicesCount      NOTIFY onDevicesCountChanged )
    Q_PROPERTY( QString bridgeIPAddress     READ bridgeIPAddress     NOTIFY bridgeIPAddressChanged )
    Q_PROPERTY( QString bridgeUsername      MEMBER m_BridgeUsername  NOTIFY bridgeUsernameChanged )
    Q_PROPERTY( QVariantMap mapModel        MEMBER m_MapModel        NOTIFY mapModelChanged )

public:
    explicit VCHue( const QString & Name, QObject * pParent = nullptr );

    const QList<HueDevice *> & devices() const { return m_Devices; }
    int onDevicesCount() const;
    const QString & bridgeIPAddress() const { return m_BridgeIPAddress; }
    const QString & bridgeUsername() const { return m_BridgeUsername; }

signals:
    void devicesChanged();
    void onDevicesCountChanged();
    void bridgeIPAddressChanged();
    void bridgeUsernameChanged();
    void mapModelChanged();

public slots:
    void refresh() override;
    void refreshGroups();
    void commandDeviceState( int iID, const QJsonObject & Parameters );

private slots:
    void handleZeroConfServiceFound( const QString & ServiceType, const QString & IPAddress );
    void handleNetworkReply( int iStatusCode, QObject * pSender, const QJsonDocument & Body );
    void updateBaseURL();

private:
    QList<HueDevice *> m_Devices;
    QHash<int, HueDevice *> m_DeviceTable;  // Key: ID, Value: device
    QString m_BridgeIPAddress;
    QString m_BridgeUsername;
    QVariantMap m_MapModel;

    QUrl m_LightsURL;
    QUrl m_GroupsURL;

    Q_DISABLE_COPY_MOVE( VCHue )
};

#endif  // VCHUE_H_
