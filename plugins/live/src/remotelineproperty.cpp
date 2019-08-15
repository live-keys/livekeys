#include "remotelineproperty.h"
#include "remoteline.h"

namespace lv{

RemoteLineProperty::RemoteLineProperty(const QString& name, RemoteLine *parent)
    : QObject(parent)
    , m_line(parent)
    , m_name(name)
{
}

void RemoteLineProperty::changed(){

}

}// namespace
