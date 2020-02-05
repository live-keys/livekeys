#ifndef QMLURLINFO_H
#define QMLURLINFO_H

#include <QObject>
#include <QUrl>

namespace lv{

class QmlUrlInfo : public QObject{

    Q_OBJECT

public:
    explicit QmlUrlInfo(QObject *parent = nullptr);

public slots:
    QString scheme(const QUrl& url){ return url.scheme(); }
    QString path(const QUrl& url) { return url.path(); }
    QString user(const QUrl& url){ return url.userName(); }
    QString password(const QUrl& url){ return url.password(); }
    QString host(const QUrl& url){ return url.host(); }
    QString toLocalFile(const QUrl& url){ return url.toLocalFile(); }
    bool isRelative(const QUrl& url) { return url.isRelative(); }
};

}// namespace

#endif // QMLURLINFO_H
