#include "bindingchannel.h"

namespace lv{

BindingChannel::BindingChannel(QObject *parent)
    : QObject(parent)
    , m_expressionPath(nullptr)
{

}

BindingChannel::~BindingChannel(){
    for ( auto it = m_paths.begin(); it != m_paths.end(); ++it )
        delete *it;
    delete m_expressionPath;
}


}// namespace
