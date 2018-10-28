#include "qmlengineinterceptor.h"
#include <QNetworkRequest>

namespace lv{

QmlEngineInterceptor::UrlInterceptor::UrlInterceptor(){

}

QUrl QmlEngineInterceptor::UrlInterceptor::intercept(const QUrl &path, QQmlAbstractUrlInterceptor::DataType){
    return path;
}

QNetworkAccessManager *QmlEngineInterceptor::Factory::create(QObject *parent){
    return new QmlEngineInterceptor(parent);
}

QNetworkReply *QmlEngineInterceptor::createRequest(
        QNetworkAccessManager::Operation op,
        const QNetworkRequest &request,
        QIODevice *outgoingData)
{
    return QNetworkAccessManager::createRequest(op, request, outgoingData);
}


void QmlEngineInterceptor::interceptEngine(QQmlEngine *engine){
    engine->setNetworkAccessManagerFactory(new QmlEngineInterceptor::Factory);
    engine->setUrlInterceptor(new QmlEngineInterceptor::UrlInterceptor);
}

}// namespace
