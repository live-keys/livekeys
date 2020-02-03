#include "qmlopening.h"

namespace lv{

QmlOpening::QmlOpening(QObject *parent)
    : QmlAct(parent)
{
}

void QmlOpening::componentComplete(){
    QmlAct::componentComplete();
    __triggerRun();
}


}// namespace
