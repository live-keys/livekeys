#ifndef LVQMLENGINEINTERCEPTOR_H
#define LVQMLENGINEINTERCEPTOR_H

#include "live/lvbaseglobal.h"

#include <QQmlEngine>
#include <QNetworkAccessManager>
#include <QQmlAbstractUrlInterceptor>
#include <QQmlNetworkAccessManagerFactory>

namespace lv{

class LV_BASE_EXPORT QmlEngineInterceptor : public QNetworkAccessManager{

public:
    class UrlInterceptor : public QQmlAbstractUrlInterceptor{
    public:
        UrlInterceptor();

        QUrl intercept(const QUrl &path, DataType type);
    };

    class Factory : public QQmlNetworkAccessManagerFactory{
    public:
        virtual QNetworkAccessManager *create(QObject *parent);
    };

public:
    explicit QmlEngineInterceptor(QObject* parent = Q_NULLPTR) : QNetworkAccessManager(parent){}

    static void interceptEngine(QQmlEngine* engine);

protected:
    virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);

};

}// namespace

#endif // LVQMLENGINEINTERCEPTOR_H
