#ifndef VCNANOLEAF_H_
#define VCNANOLEAF_H_

#include <QVariant>

#include "vcplugin.h"

class VCNanoleaf final : public VCPlugin
{
    Q_OBJECT
    Q_PROPERTY( QString name            READ name            NOTIFY nameChanged )
    Q_PROPERTY( bool isOn               READ isOn            NOTIFY isOnChanged )
    Q_PROPERTY( QVariantList effects    READ effects         NOTIFY effectsChanged )
    Q_PROPERTY( QString selectedEffect  READ selectedEffect  NOTIFY selectedEffectChanged )
    Q_PROPERTY( QString ipAddress       READ ipAddress       NOTIFY ipAddressChanged )
    Q_PROPERTY( QString authToken       MEMBER m_AuthToken   NOTIFY authTokenChanged )
    Q_PROPERTY( QVariantList mapPoint   MEMBER m_MapPoint    NOTIFY mapPointChanged )

public:
    explicit VCNanoleaf( const QString & Name, QObject * pParent = nullptr );

    const QString & name() const { return m_Name; }
    bool isOn() const { return m_bIsOn; }
    const QVariantList & effects() const { return m_Effects; }
    const QString & selectedEffect() const { return m_SelectedEffect; }
    const QString & ipAddress() const { return m_IPAddress; }

signals:
    void nameChanged();
    void isOnChanged();
    void effectsChanged();
    void selectedEffectChanged();
    void ipAddressChanged();
    void authTokenChanged();
    void mapPointChanged();

public slots:
    void refresh() override;
    void commandPower( bool bOn );
    void selectEffect( const QString & Effect );

private slots:
    void handleZeroConfServiceFound( const QString & ServiceType, const QString & IPAddress );
    void handleNetworkReply( int iStatusCode, QObject * pSender, const QJsonDocument & Body );
    void updateBaseURL();
    void refreshEffects();

private:
    QString Name;
    bool m_bIsOn;
    int m_iCommandedPower;
    QVariantList m_Effects;
    QTimer m_EffectsRefreshTimer;
    QString m_SelectedEffect;
    QString m_CommandedEffect;
    QString m_IPAddress;
    QString m_AuthToken;
    QVariantList m_MapPoint;

    QString m_BaseURL;

    Q_DISABLE_COPY_MOVE( VCNanoleaf )
};

#endif  // VCNANOLEAF_H_
