#ifndef VCPLUGIN_H_
#define VCPLUGIN_H_

#include <QTimer>

class VCPlugin : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString pluginName    READ pluginName                                CONSTANT )
    Q_PROPERTY( int updateInterval    READ updateInterval  WRITE setUpdateInterval   NOTIFY updateIntervalChanged )
    Q_PROPERTY( bool isActive         READ isActive        WRITE setActive           NOTIFY isActiveChanged )

public:
    explicit VCPlugin( const QString & Name, QObject * pParent = nullptr );

    const QString & pluginName() const { return m_PluginName; }
    int updateInterval() const { return m_iUpdateInterval; }
    void setUpdateInterval( int iValue );
    bool isActive() const { return m_bIsActive; }
    void setActive( bool bValue );

signals:
    void updateIntervalChanged();
    void isActiveChanged();

public slots:
    virtual void refresh() = 0;

protected:
    QString m_PluginName;
    int m_iUpdateInterval;
    QTimer m_UpdateTimer;
    bool m_bIsActive;

    Q_DISABLE_COPY_MOVE( VCPlugin )
};

#endif  // VCPLUGIN_H_
