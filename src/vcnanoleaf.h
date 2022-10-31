#ifndef VCNANOLEAF_H_
#define VCNANOLEAF_H_

#include <QVariant>

#include "vcplugin.h"

class VCNanoleaf final : public VCPlugin {
    Q_OBJECT

    // clang-format off
    Q_PROPERTY(QString name                               READ name            NOTIFY nameChanged)
    Q_PROPERTY(bool isOn                                  READ isOn            NOTIFY isOnChanged)
    Q_PROPERTY(QVariantList effects                       READ effects         NOTIFY effectsChanged)
    Q_PROPERTY(QString selectedEffect                     READ selectedEffect  NOTIFY selectedEffectChanged)
    Q_PROPERTY(QString ipAddress                          READ ipAddress       NOTIFY ipAddressChanged)
    Q_PROPERTY(QString authToken       MEMBER authToken_                       NOTIFY authTokenChanged)
    Q_PROPERTY(QVariantList mapPoint   MEMBER mapPoint_                        NOTIFY mapPointChanged)
    // clang-format on

 public:
    explicit VCNanoleaf(const QString& name, QObject* parent = nullptr);

    const QString& name() const { return name_; }
    bool isOn() const { return isOn_; }
    const QVariantList& effects() const { return effects_; }
    const QString& selectedEffect() const { return selectedEffect_; }
    const QString& ipAddress() const { return ipAddress_; }

    Q_INVOKABLE void commandPower(bool on);
    Q_INVOKABLE void selectEffect(const QString& effect);

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
    void refreshEffects();

 private slots:
    void handleZeroConfServiceFound(const QString& serviceType, const QString& ipAddress);
    void handleNetworkReply(int statusCode, QObject* sender, const QJsonDocument& body);
    void updateBaseURL();

 private:
    QString name_;
    bool isOn_;
    int commandedPower_;
    QVariantList effects_;
    QString selectedEffect_;
    QString commandedEffect_;
    QString ipAddress_;
    QString authToken_;
    QVariantList mapPoint_;

    QString baseURL_;

    Q_DISABLE_COPY_MOVE(VCNanoleaf)
};

#endif  // VCNANOLEAF_H_
