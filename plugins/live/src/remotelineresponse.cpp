#include "remotelineresponse.h"
#include "tcplinesocket.h"

namespace lv{

RemoteLineResponse::RemoteLineResponse(QObject *parent)
    : QObject(parent)
{
}

void RemoteLineResponse::onResponse(std::function<void (const QString &, const QVariant &)> callback){
    m_responseCallback = callback;
}

void RemoteLineResponse::send(const QString &propertyName, const QVariant &value){
    m_responseCallback(propertyName, value);
}

}// namespace
