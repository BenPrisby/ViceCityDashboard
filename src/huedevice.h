#ifndef HUEDEVICE_H_
#define HUEDEVICE_H_

#include <QJsonDocument>
#include <QJsonObject>

class HueDevice : public QObject
{
    Q_OBJECT
    Q_PROPERTY( int id               READ id                          NOTIFY idChanged )
    Q_PROPERTY( QString name         READ name                        NOTIFY nameChanged )
    Q_PROPERTY( QString type         READ type                        NOTIFY typeChanged )
    Q_PROPERTY( QString productName  READ productName                 NOTIFY productNameChanged )
    Q_PROPERTY( bool isReachable     READ isReachable                 NOTIFY isReachableChanged )
    Q_PROPERTY( bool isOn            READ isOn                        NOTIFY isOnChanged )
    Q_PROPERTY( QString room         READ room         WRITE setRoom  NOTIFY roomChanged )

public:
    explicit HueDevice( int iID, QObject * pParent = nullptr );

    int id() const { return m_iID; }
    const QString & name() const { return m_Name; }
    const QString & type() const { return m_Type; }
    const QString & productName() const { return m_ProductName; }
    bool isReachable() const { return m_bIsReachable; }
    bool isOn() const { return m_bIsOn; }
    const QString & room() const { return m_Room; }
    void setRoom( const QString & Value );

signals:
    void idChanged();
    void nameChanged();
    void typeChanged();
    void productNameChanged();
    void isReachableChanged();
    void isOnChanged();
    void roomChanged();

public slots:
    void commandPower( bool bOn );
    void handleResponse( const QJsonDocument & Response );

protected:
    int m_iID;
    QString m_Name;
    QString m_Type;
    QString m_ProductName;
    bool m_bIsReachable;
    bool m_bIsOn;
    QString m_Room;

    virtual void handleStateData( const QJsonObject & State );

private:
    void handleResponseData( const QJsonObject & Data );

    Q_DISABLE_COPY_MOVE( HueDevice )
};

#endif  // HUEDEVICE_H_
