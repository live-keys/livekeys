#include "scenario.h"

namespace lv{ namespace el{

Scenario::Scenario(Engine* engine)
    : Element(engine)
    , m_run(engine)
{
}

Scenario::~Scenario(){
}

void Scenario::setDescribe(const std::string &describe){
    if ( m_describe != describe ){
        m_describe = describe;
        describeChanged();
    }
}

void Scenario::setRun(Callable c){
    m_run = c;
    runChanged();
}

}} // namespace lv, el
