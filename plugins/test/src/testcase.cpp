#include "testcase.h"
#include "live/elements/object.h"
#include "live/visuallog.h"

namespace lv{ namespace el{

TestCase::TestCase(Engine* engine)
    : Element(engine)
    , m_beforeEach(engine)
    , m_beforeAll(engine)
    , m_afterEach(engine)
    , m_afterAll(engine)
    , m_ready(false)
{
}

TestCase::~TestCase(){
}

LocalValue TestCase::children(){
    Object o = Object::createArray(engine(), static_cast<int>(m_data.size()));
    LocalArray la(o);
    for ( int i = 0; i < static_cast<int>(m_data.size()); ++i ){
        la.set(i, LocalValue(engine(), m_data[static_cast<size_t>(i)]));
    }
    return LocalValue(engine(), o);
}

void TestCase::setChildren(LocalValue children){
    for( Element* e: m_data ){
        e->setParent(nullptr);
    }
    m_data.clear();
    if ( children.isArray() ){
        LocalArray la(engine(), children);
        for ( int i = 0; i < la.length(); ++i ){
            LocalValue lval = la.get(i);
            if ( lval.isElement() ){
                Element* el = lval.toElement(engine());
                if (el){
                    el->setParent(this);
                    m_data.push_back(el);
                }
            }
        }
    }
    childrenChanged();
}

const std::vector<Element*> TestCase::data() const{
    return m_data;
}

void TestCase::onReady(){
    m_ready = true;

    //TODO: run scenarios
    // run before all
    // for each scenario
     // run before each
     // run
     // run after each
    // run after all
}

}} // namespace lv, el
