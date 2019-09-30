#include "qmlfollowup.h"
#include "qmlcontainer.h"

#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

namespace lv{

QmlFollowUp::QmlFollowUp(QObject *parent)
    : Act(parent)
    , m_prevAct(nullptr)
{
}

void QmlFollowUp::_whenPrevChanged(){
    m_prev = m_prevAct->result();
    emit Act::run();
}

void QmlFollowUp::componentComplete(){
    QmlContainer* parentContainer = qobject_cast<QmlContainer*>(parent());
    if ( !parentContainer ){
        Exception e = CREATE_EXCEPTION(
            lv::Exception, "FollowUp: Parent needs to be of container type.", Exception::toCode("~cast")
        );
        lv::ViewContext::instance().engine()->throwError(&e, this);
        Act::componentComplete();
        return;
    }

    m_prevAct = qobject_cast<Act*>(parentContainer->prevChild(this));
    if ( !m_prevAct ){
        Exception e = CREATE_EXCEPTION(
            lv::Exception, "FollowUp: No previous act found.", Exception::toCode("~act")
        );
        lv::ViewContext::instance().engine()->throwError(&e, this);
        Act::componentComplete();
        return;
    }

    connect(m_prevAct, &Act::resultChanged, this, &QmlFollowUp::_whenPrevChanged);

    Act::componentComplete();

    if ( m_prevAct->result().isValid() ){
        emit Act::run();
    }
}

}// namespace
