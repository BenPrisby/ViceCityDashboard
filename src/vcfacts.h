#ifndef VCFACTS_H_
#define VCFACTS_H_

#include <QUrl>

#include "vcplugin.h"

class VCFacts : public VCPlugin
{
    Q_OBJECT
    Q_PROPERTY( QString fact  READ fact  NOTIFY factChanged )

public:
    VCFacts( const QString & Name, QObject * pParent = nullptr );

    const QString & fact() const { return m_Fact; }

signals:
    void factChanged();

public slots:
    void refresh() override;

private slots:
    void handleNetworkReply( int iStatusCode, QObject * pSender, const QJsonDocument & Body );

private:
    QString m_Fact;

    QUrl m_RequestURL;

    Q_DISABLE_COPY_MOVE( VCFacts )
};

#endif  // VCFACTS_H_
