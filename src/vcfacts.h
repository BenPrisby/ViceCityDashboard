#ifndef VCFACTS_H_
#define VCFACTS_H_

#include <QUrl>

#include "vcplugin.h"

class VCFacts final : public VCPlugin
{
    Q_OBJECT
    Q_PROPERTY( QString fact  READ fact  NOTIFY factChanged )

public:
    VCFacts( const QString & name, QObject * parent = nullptr );

    const QString & fact() const { return fact_; }

signals:
    void factChanged();

public slots:
    void refresh() override;

private slots:
    void handleNetworkReply( int statusCode, QObject * sender, const QJsonDocument & body );

private:
    QString fact_;

    QUrl requestURL_;

    Q_DISABLE_COPY_MOVE( VCFacts )
};

#endif  // VCFACTS_H_
