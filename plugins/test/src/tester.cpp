#include "tester.h"

#include <QFileInfo>

namespace lv{ namespace el{

Tester::Tester(Engine* engine)
    : Element(engine)
{
}

Tester::~Tester(){
}

void Tester::assertImpl(const Function::CallInfo&){

}

void Tester::assertThrowsImpl(const Function::CallInfo&){

}

}} // namespace lv, el
