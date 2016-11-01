#include "qlivecvincubationcontroller.h"

QLiveCVIncubationController::QLiveCVIncubationController(QObject *parent)
    : QObject(parent)
{
    startTimer(15);
}

QLiveCVIncubationController::~QLiveCVIncubationController(){

}

void QLiveCVIncubationController::timerEvent(QTimerEvent *){
    incubateFor(14);
}
