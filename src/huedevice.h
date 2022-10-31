#ifndef HUEDEVICE_H_
#define HUEDEVICE_H_

#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QString>

class HueDevice : public QObject {
    Q_OBJECT

    // clang-format off
    Q_PROPERTY(int id               READ id                          NOTIFY idChanged)
    Q_PROPERTY(QString name         READ name                        NOTIFY nameChanged)
    Q_PROPERTY(QString type         READ type                        NOTIFY typeChanged)
    Q_PROPERTY(QString productName  READ productName                 NOTIFY productNameChanged)
    Q_PROPERTY(bool isReachable     READ isReachable                 NOTIFY isReachableChanged)
    Q_PROPERTY(bool isOn            READ isOn                        NOTIFY isOnChanged)
    Q_PROPERTY(QString room         READ room         WRITE setRoom  NOTIFY roomChanged)
    // clang-format on

 public:
    explicit HueDevice(int id, QObject* parent = nullptr);

    int id() const { return id_; }
    const QString& name() const { return name_; }
    const QString& type() const { return type_; }
    const QString& productName() const { return productName_; }
    bool isReachable() const { return isReachable_; }
    bool isOn() const { return isOn_; }
    const QString& room() const { return room_; }
    void setRoom(const QString& value);

 signals:
    void idChanged();
    void nameChanged();
    void typeChanged();
    void productNameChanged();
    void isReachableChanged();
    void isOnChanged();
    void roomChanged();

 public slots:
    void commandPower(bool on);
    void handleResponse(const QJsonDocument& response);

 protected:
    int id_;
    QString name_;
    QString type_;
    QString productName_;
    bool isReachable_;
    bool isOn_;
    QString room_;

    virtual void handleStateData(const QJsonObject& state);

 private:
    void handleResponseData(const QJsonObject& data);

    Q_DISABLE_COPY_MOVE(HueDevice)
};

#endif  // HUEDEVICE_H_
