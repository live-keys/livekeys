#include "bindingchannel.h"
#include "live/codepalette.h"
#include "live/qmleditfragment.h"

namespace lv{

BindingChannel::BindingChannel(QmlEditFragment *fragment, QObject *parent)
    : QObject(parent)
    , m_fragment(fragment)
    , m_expressionPath(nullptr)
{
}

BindingChannel::~BindingChannel(){
    for ( auto it = m_paths.begin(); it != m_paths.end(); ++it )
        delete *it;
    delete m_expressionPath;
}


}// namespace
