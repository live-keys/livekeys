#include "qmlbuild.h"
#include "live/visuallogqt.h"

namespace lv{

QmlBuild::QmlBuild(Runnable *runnable, QObject *parent)
    : QObject(parent)
    , m_runnable(runnable)
    , m_state(QmlBuild::Compiling)
{
}

QmlBuild::~QmlBuild(){
}

void QmlBuild::setState(QmlBuild::State state){
    if ( m_state == state )
        return;

    m_state = state;
    if ( m_state == QmlBuild::Ready )
        emit ready();
    if ( m_state == QmlBuild::Removed )
        emit remove();
    if ( m_state == QmlBuild::Errored ){
        emit error();
    }
}

}// namespace
