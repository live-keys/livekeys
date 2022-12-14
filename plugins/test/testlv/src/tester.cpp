#include "tester.h"
#include "scenario.h"

#include <QFileInfo>

namespace lv{ namespace el{

Tester::Tester(Engine* engine)
    : Element(engine)
{
}

Tester::~Tester(){
}

void Tester::assertImpl(const Function::CallInfo& c){
    if ( c.length() == 0 ){
        Exception e = CREATE_EXCEPTION(Exception, "Invalid number of arguments given at scenario: ", Exception::toCode("~Tester"));
        engine()->throwError(&e, this);
    } else {
        if ( !c.at(0).toBool(engine()) ){
            std::string message;
            if ( c.length() > 1 ){
                message = "Assertion \'" + c.at(1).toStdString(engine()) + "\' failed.";
            } else {
                message = "Assertion failed.";
            }

            Exception e = CREATE_EXCEPTION(Exception, message, Exception::toCode("~Assertion"));
            engine()->throwError(&e, this);
        }
    }
}

void Tester::assertThrowsImpl(const Function::CallInfo& c){
    if ( c.length() == 0 ){
        Exception e = CREATE_EXCEPTION(Exception, "Invalid number of arguments given at scenario: ", Exception::toCode("~Tester"));
        engine()->throwError(&e, this);
    } else {
        bool except = false;
        engine()->tryCatch([c, this](){
            c.at(0).toCallable(engine()).call(engine(), Function::Parameters(0));
        }, [&except, c](const Engine::CatchData&){
            //TODO: errorType = cd.at(1).type() || Error;
            //TODO: if cd.exceptionObject instanceof errorType -> except = true;
        });

        if (!except){
            std::string message;
            if ( c.length() > 2 ){
                message = "Assertion \'" + c.at(2).toStdString(engine()) + "\' failed.";
            } else {
                message = "Assertion failed.";
            }

            Exception e = CREATE_EXCEPTION(Exception, message, Exception::toCode("~Assertion"));
            engine()->throwError(&e, this);
        }
    }
}

}} // namespace lv, el
