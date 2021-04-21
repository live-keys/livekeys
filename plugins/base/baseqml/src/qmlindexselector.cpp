#include "qmlindexselector.h"
#include "live/visuallogqt.h"

namespace lv{

QmlIndexSelector::QmlIndexSelector(QObject *parent)
    : QObject(parent)
    , m_index(-1)
{
}

void QmlIndexSelector::setList(QJSValue list){
    m_list = list;
    emit listChanged();

    if ( m_index != -1 && !m_list.isNull() ){
        m_current = m_list.property(static_cast<unsigned int>(m_index));
        emit currentChanged();
    }
}

void QmlIndexSelector::setIndex(int index){
    if (m_index == index)
        return;

    m_index = index;
    emit indexChanged();

    if ( m_index != -1 && !m_list.isNull() ){
        m_current = m_list.property(static_cast<unsigned int>(m_index));
        emit currentChanged();
    }
}

}// namespace
