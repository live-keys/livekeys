#include "qmlopening.h"

namespace lv{

QmlOpening::QmlOpening(QObject *parent)
    : Act(parent)
{
}

void QmlOpening::componentComplete(){
    Act::componentComplete();
    emit Act::run();
}


}// namespace
