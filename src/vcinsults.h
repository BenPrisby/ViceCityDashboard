#ifndef VCINSULTS_H_
#define VCINSULTS_H_

#include <QUrl>

#include "vcplugin.h"

class VCInsults : public VCPlugin
{
    Q_OBJECT
    Q_PROPERTY( QString insult  READ insult  NOTIFY insultChanged )

public:
    VCInsults( const QString & Name, QObject * pParent = nullptr );

    const QString & insult() const { return m_Insult; }

signals:
    void insultChanged();

public slots:
    void refresh() override;

private slots:
    void handleNetworkReply( int iStatusCode, QObject * pSender, const QByteArray & Body );

private:
    QString m_Insult;

    QUrl m_RequestURL;

    QString fromHtmlEscaped( QString Text );

    Q_DISABLE_COPY_MOVE( VCInsults )
};

#endif  // VCINSULTS_H_
