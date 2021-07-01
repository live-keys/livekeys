#include "qmlstreamact.h"

namespace lv{

QmlStreamAct::QmlStreamAct(QObject *parent)
    : QObject(parent)
    , m_input(nullptr)
    , m_output(nullptr)
    , m_act(nullptr)
    , m_isRunning(false)
{
}

QmlStreamAct::~QmlStreamAct(){
    delete m_output;
}

void QmlStreamAct::onStreamValue(const QJSValue &val){
    if ( m_isRunning ){
        return;
    }

    if ( !m_act || !m_propertyMeta.isValid() )
        return;

    m_propertyMeta.write(m_act, val.toVariant());
    m_act->exec();
    m_isRunning = true;

    if ( m_input->provider() )
        m_input->provider()->wait();
}

void QmlStreamAct::setInput(QmlStream *stream){
    if (m_input == stream)
        return;

    if ( m_input ){
        m_input->unsubscribeObject(this);
        delete m_output;
        m_output = nullptr;
    }

    m_input = stream;
    if ( m_input ){
        m_input->forward(this, &QmlStreamAct::streamHandler);
        m_output = new QmlStream(this, this);
    }

    emit inputChanged();
    emit outChanged();
}

void QmlStreamAct::streamHandler(QObject *that, const QJSValue &val){
    QmlStreamAct* streamAct = static_cast<QmlStreamAct*>(that);
    streamAct->onStreamValue(val);
}

void QmlStreamAct::__actResultReady(){
    m_isRunning = false;
    if ( m_input->provider() )
        m_input->provider()->resume();
}

void QmlStreamAct::initMetaProperty(){
    if ( m_actProperty.isEmpty() )
        return;

    const QMetaObject* meta = m_act->metaObject();

    for ( int i = 0; i < meta->propertyCount(); i++ ){
        QMetaProperty property = meta->property(i);
        if ( property.name() == m_actProperty ){
            m_propertyMeta = property;
            return;
        }
    }
}

void QmlStreamAct::wait(){
    if ( m_input && m_input->provider() ){
        m_input->provider()->wait();
    }
}

void QmlStreamAct::resume(){
    if ( m_input && m_input->provider() ){
        m_input->provider()->resume();
    }
}

void QmlStreamAct::setAct(lv::QmlAct *act){
    if (m_act == act)
        return;

    if ( m_act ){
        disconnect(m_act, &QmlAct::resultChanged, this, &QmlStreamAct::__actResultReady);
    }

    m_act = act;
    if ( m_act ){
        connect(m_act, &QmlAct::resultChanged, this, &QmlStreamAct::__actResultReady);
        m_act->setTrigger(QmlAct::Manual);
        initMetaProperty();
    }

    m_act = act;
    emit actChanged();
}

void QmlStreamAct::setActProperty(const QString &argument){
    if (m_actProperty == argument)
        return;

    m_actProperty = argument;
    if ( m_act )
        initMetaProperty();

    emit actPropertyChanged();
}


}// namespace
