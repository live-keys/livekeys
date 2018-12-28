#ifndef LVQMLENGINEINTERCEPTOR_H
#define LVQMLENGINEINTERCEPTOR_H

#include "live/lvviewglobal.h"
#include "live/packagegraph.h"
#include "live/viewengine.h"

#include <QQmlEngine>
#include <QNetworkAccessManager>
#include <QQmlAbstractUrlInterceptor>
#include <QQmlNetworkAccessManagerFactory>

namespace lv{

class LV_VIEW_EXPORT QmlEngineInterceptor : public QNetworkAccessManager{

public:
    class UrlInterceptor : public QQmlAbstractUrlInterceptor{
    public:
        UrlInterceptor(ViewEngine* engine, PackageGraph* PackageGraph);

        QUrl intercept(const QUrl &path, DataType type);

    private:
        ViewEngine*   m_engine;
        PackageGraph* m_packageGraph;
    };

    class Factory : public QQmlNetworkAccessManagerFactory{
    public:
        virtual QNetworkAccessManager *create(QObject *parent);
    };

public:
    explicit QmlEngineInterceptor(QObject* parent = Q_NULLPTR) : QNetworkAccessManager(parent){}

    static void interceptEngine(ViewEngine* engine, PackageGraph* packageGraph);

protected:
    virtual QNetworkReply* createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData);

};

}// namespace

#endif // LVQMLENGINEINTERCEPTOR_H
