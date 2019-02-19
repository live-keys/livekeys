#include "group.h"

namespace lv{

Group::Group(QObject *parent)
    : QObject(parent)
    , m_isComponentComplete(false)
{

}

Group::~Group(){

}

void Group::componentComplete(){
    m_isComponentComplete = true;
    emit complete();
}

}// namespace
