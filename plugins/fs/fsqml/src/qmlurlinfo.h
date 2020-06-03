#ifndef LVQMLURLINFO_H
#define LVQMLURLINFO_H

#include <QObject>
#include <QUrl>
#include <QJSValue>

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
    QString fragment(const QUrl& url){ return url.fragment(); }
    QUrl urlFromLocalFile(const QString& filePath, const QJSValue& opt = QJSValue());
    bool hasFragment(const QUrl& url){ return url.hasFragment(); }
    bool isRelative(const QUrl& url) { return url.isRelative(); }
};

}// namespace

#endif // LVQMLURLINFO_H
