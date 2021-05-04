#include "testcase.h"
#include "tester.h"
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

ScopedValue TestCase::children(){
    Object o = Object::createArray(engine(), static_cast<int>(m_data.size()));
    Object::ArrayAccessor la(o);
    for ( int i = 0; i < static_cast<int>(m_data.size()); ++i ){
        la.set(engine(), i, ScopedValue(engine(), m_data[static_cast<size_t>(i)]));
    }
    return ScopedValue(engine(), o);
}

void TestCase::setChildren(ScopedValue children){
    for( Element* e: m_data ){
        e->setParent(nullptr);
    }
    m_data.clear();
    if ( children.isArray() ){
        Object::ArrayAccessor la(engine(), children);
        for ( int i = 0; i < la.length(); ++i ){
            ScopedValue lval = la.get(engine(), i);
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
    if ( !parent() ){
        exec();
    }
}

int TestCase::exec(){
    Engine* e = engine();

    if ( !m_beforeAll.isNull() ){
        e->tryCatch([e, this](){
            m_beforeAll.call(e, Function::Parameters(0));
        }, [](const Engine::CatchData& cd){
            vlog().e() << "TestCase 'BeforeAll' Error in \'" << cd.fileName() << "\': " << cd.message() << cd.stack();
            return -1;
        });
    }

    for( auto it = m_data.begin(); it != m_data.end(); ++it ){
        Element* el = *it;
        Scenario* scenario = el->cast<Scenario>();
        if ( scenario ){

            if ( !m_beforeEach.isNull() ){
                //TODO: Catch engine exception
                m_beforeEach.call(engine(), Function::Parameters(0));
            }

            {
                bool except = false;

                Tester* tester = new Tester(engine());
                tester->setParent(scenario);

                engine()->tryCatch([scenario, tester, this](){

                    Function::Parameters p(1);
                    p.assign(0, ScopedValue(engine(), tester));

                    //TODO: Check if run() is null
                    vlog() << scenario->run().isNull();
                    if ( !scenario->run().isNull() )
                        scenario->run().call(scenario, p);

                }, [&except, scenario](const Engine::CatchData& cd){
                    except = true;
                    vlog("test").i() << " * " << scenario->describe() <<  " : " << "FAIL";
                    vlog("test").i() << "     " << cd.message();
                });

                if ( !except )
                    vlog("test").i() << " * " << scenario->describe() <<  " : " << "PASS";

                delete tester;
            }

            if ( !m_afterEach.isNull() ){
                //TODO: Catch engine exception
                m_afterEach.call(engine(), Function::Parameters(0));
            }
        }
    }

    if ( !m_afterAll.isNull() ){
        e->tryCatch([this](){
            m_afterAll.call(engine(), Function::Parameters(0));
        }, [](const Engine::CatchData& cd){
            vlog().e() << "TestCase 'BeforeAll' Error in \'" << cd.fileName() << "\': " << cd.message() << cd.stack();
            return -2;
        });
    }
    return 0;
}

}} // namespace lv, el
