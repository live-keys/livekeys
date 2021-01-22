#include "qmlpropertylog.h"
#include "live/visuallogqt.h"
#include "live/visuallogqmlobject.h"

namespace lv{

QmlPropertyLog::QmlPropertyLog(QObject *parent)
    : QObject(parent)
{
}

QmlPropertyLog::~QmlPropertyLog(){
}

void QmlPropertyLog::setInput(QJSValue input){
    VisualLog vl(VisualLog::MessageInfo::Info);
    VisualLogQmlObject::logValue(vl, input);
}

}// namespace
