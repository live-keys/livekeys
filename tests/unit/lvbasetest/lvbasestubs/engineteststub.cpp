#include "engineteststub.h"
#include "live/exception.h"

EngineTestStub::EngineTestStub(QObject *parent)
    : QObject(parent)
{

}

void EngineTestStub::throwException(){
    THROW_EXCEPTION(lcv::Exception, "Exception stub.", 0);
}

void EngineTestStub::throwJsException(){
    //TODO
}
