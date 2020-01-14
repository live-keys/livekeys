#include "qmlfollowup.h"
#include "qmlcontainer.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

namespace lv{

QmlFollowUp::QmlFollowUp(QObject *parent)
    : QmlAct(parent)
    , m_prevAct(nullptr)
{
}

void QmlFollowUp::_whenPrevChanged(){
    m_prev = m_prevAct->result();
    __triggerRun();
}

void QmlFollowUp::componentComplete(){
    QmlContainer* parentContainer = qobject_cast<QmlContainer*>(parent());
    if ( !parentContainer ){
        Exception e = CREATE_EXCEPTION(
            lv::Exception, "FollowUp: Parent needs to be of container type.", Exception::toCode("~cast")
        );
        lv::ViewContext::instance().engine()->throwError(&e, this);
        QmlAct::componentComplete();
        return;
    }

    m_prevAct = qobject_cast<QmlAct*>(parentContainer->prevChild(this));
    if ( !m_prevAct ){
        Exception e = CREATE_EXCEPTION(
            lv::Exception, "FollowUp: No previous act found.", Exception::toCode("~act")
        );
        lv::ViewContext::instance().engine()->throwError(&e, this);
        QmlAct::componentComplete();
        return;
    }

    connect(m_prevAct, &QmlAct::resultChanged, this, &QmlFollowUp::_whenPrevChanged);

    QmlAct::componentComplete();

    if ( !m_prevAct->result().isUndefined() ){
        __triggerRun();
    }
}

}// namespace
