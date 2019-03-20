#include "tcplineproperty.h"
#include "tcpline.h"

namespace lv{

TcpLineProperty::TcpLineProperty(const QString& name, TcpLine *parent)
    : QObject(parent)
    , m_line(parent)
    , m_name(name)
{
}

void TcpLineProperty::changed(){

}

}// namespace
