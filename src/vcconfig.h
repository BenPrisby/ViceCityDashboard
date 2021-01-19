#ifndef VCCONFIG_H_
#define VCCONFIG_H_

#include <QMetaMethod>
#include <QVariant>

using KeyContext = QPair<QObject *, QString>;  // Object, property name

class VCConfig final : public QObject
{
    Q_OBJECT

public:
    static VCConfig * instance();

    bool load( const QString & Path );

public slots:
    bool save();

private:
    explicit VCConfig( QObject * pParent = nullptr );

    QString m_Path;
    QStringList m_Keys;
    QMetaMethod m_SaveMethod;

    KeyContext keyToContext( const QString & Key );
};

#endif  // VCCONFIG_H_
