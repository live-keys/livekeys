#include "eventrelay.h"

#include <QCoreApplication>
#include <QKeyEvent>

namespace lv{

EventRelay::EventRelay(QObject *parent)
    : QObject(parent)
{
}

void EventRelay::relayKeyEvent(QObject *object, int key, int modifiers){
    QKeyEvent event(QKeyEvent::KeyPress, key, static_cast<Qt::KeyboardModifiers>(modifiers));
    QCoreApplication::sendEvent(object, &event);
}

}// namespace
