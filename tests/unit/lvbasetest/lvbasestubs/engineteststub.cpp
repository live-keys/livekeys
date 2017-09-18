#include "engineteststub.h"
#include "live/exception.h"
#include <QQmlContext>
#include <QQmlEngine>

EngineTestStub::EngineTestStub(QObject *parent)
    : QObject(parent)
{
}

void EngineTestStub::throwException(){
    THROW_EXCEPTION(lcv::Exception, "Exception stub.", 0);
}

void EngineTestStub::throwJsError(){
    QObject* engineObj = qmlContext(this)->contextProperty("engine").value<QObject*>();
    lcv::Engine* engine = qobject_cast<lcv::Engine*>(engineObj);
    if ( !engine )
        return;

    lcv::Exception exception = lcv::Exception::create<lcv::Exception>(
        "JSTest", 1, "enginetest.cpp", 100, "jsExceptionInObjectTest"
    );
    engine->throwError(&exception, this);
}

void EngineTestStub::throwJsWarning(){
    QObject* engineObj = qmlContext(this)->contextProperty("engine").value<QObject*>();
    lcv::Engine* engine = qobject_cast<lcv::Engine*>(engineObj);
    if ( !engine )
        return;

    engine->throwWarning("JSTest", this, "enginetest.cpp", 100);
}
