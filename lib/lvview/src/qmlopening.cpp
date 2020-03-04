#include "qmlopening.h"

namespace lv{

QmlOpening::QmlOpening(QObject *parent)
    : QmlAct(parent)
{
}

void QmlOpening::componentComplete(){
    QmlAct::componentComplete();
    exec();
}


}// namespace
