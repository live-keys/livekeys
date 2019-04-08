#include "tcplineresponse.h"
#include "tcplinesocket.h"

namespace lv{

TcpLineResponse::TcpLineResponse(TcpLineSocket *parent)
    : QObject(parent)
    , m_socket(parent)
{
}

void TcpLineResponse::send(const QString &propertyName, const QVariant &value){
    m_socket->responseValueChanged(propertyName, value);
}

}// namespace
