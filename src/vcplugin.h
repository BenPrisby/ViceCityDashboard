#ifndef VCPLUGIN_H_
#define VCPLUGIN_H_

#include <QObject>
#include <QString>
#include <QTimer>

class VCPlugin : public QObject {
    Q_OBJECT

    // clang-format off
    Q_PROPERTY(QString pluginName  READ pluginName                               CONSTANT)
    Q_PROPERTY(int updateInterval  READ updateInterval  WRITE setUpdateInterval  NOTIFY updateIntervalChanged)
    Q_PROPERTY(bool isActive       READ isActive        WRITE setActive          NOTIFY isActiveChanged)
    // clang-format on

 public:
    explicit VCPlugin(const QString& name, QObject* parent = nullptr);

    const QString& pluginName() const { return pluginName_; }
    int updateInterval() const { return updateInterval_; }
    void setUpdateInterval(int value);
    bool isActive() const { return isActive_; }
    void setActive(bool value);

 signals:
    void updateIntervalChanged();
    void isActiveChanged();

 public slots:
    virtual void refresh() = 0;

 protected:
    QString pluginName_;
    int updateInterval_;
    QTimer updateTimer_;
    bool isActive_;

    Q_DISABLE_COPY_MOVE(VCPlugin)
};

#endif  // VCPLUGIN_H_
