#ifndef VCCONFIG_H_
#define VCCONFIG_H_

#include <QList>
#include <QMetaMethod>
#include <QObject>
#include <QString>
#include <QVariant>

using KeyContext = QPair<QObject *, QString>;  // Object, property name

class VCConfig final : public QObject {
    Q_OBJECT

 public:
    static VCConfig *instance();

    bool load(const QString &path);

 public slots:
    bool save();

 private:
    explicit VCConfig(QObject *parent = nullptr);

    QString path_;
    QStringList keys_;
    QMetaMethod saveMethod_;

    KeyContext keyToContext(const QString &key);

    Q_DISABLE_COPY_MOVE(VCConfig)
};

#endif  // VCCONFIG_H_
