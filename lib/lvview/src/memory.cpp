#include "memory.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"
#include "live/exception.h"

#include <QQmlContext>
#include <QQmlEngine>
#include <QJSValue>

namespace lv{

Memory::Memory(QObject *parent)
    : QObject(parent)
{
}

Memory::~Memory(){
    totalSpace() = false;
}

Memory *Memory::i(){
    return ViewContext::instance().engine()->memory();
}

void Memory::gc(){
    QJSValue gcFn = ViewContext::instance().engine()->engine()->globalObject().property("gc");
    gcFn.call();
}

void Memory::reloc(){
    gc();
}

void Memory::recycleSize(Shared *o, int size) const{
    o->recycleSize(size);
}

void Memory::logDeletion(QObject *object){
    connect(object, &QObject::destroyed, [object](){
        vlog("main") << "ViewEngine: Object Destroyed:" << object;
    });
}

}// namespace
