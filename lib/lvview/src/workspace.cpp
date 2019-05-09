#include "workspace.h"

namespace lv{


Workspace::Message::Message(Workspace::Message::Type type, const MLNode &data)
    : m_type(type)
    , m_data(data)
{
}

Workspace::Workspace(QObject *parent)
    : QObject(parent)
{
}

Workspace::~Workspace(){
}


}// namespace
