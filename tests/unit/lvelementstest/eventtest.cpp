#include "eventtest.h"
#include "live/elements/event.h"
#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/modulelibrary.h"

Q_TEST_RUNNER_REGISTER(EventTest);

using namespace lv;
using namespace lv::el;

class EventEmitter : public Element{

public:
    EventEmitter(Engine* engine) : Element(engine){}

    Event valueChanged(double val){
        return notify(eventId(&EventEmitter::valueChanged), val);
    }

    void setValue(double value){
        m_value = value;
        valueChanged(m_value);
    }

private:
    double m_value;
};

class EventObserver : public Element{

public:
    EventObserver(Engine* engine) : Element(engine), m_value(0){}

    void observeValue(double newValue){ m_value = newValue; }
    double value() const{ return m_value; }

private:
    double m_value;
};

EventTest::EventTest(QObject *parent)
    : QObject(parent)
{
}

void EventTest::initTestCase(){
}

void EventTest::eventGenerationTest(){
    Event::Id eid = EventIdGenerator::eventId(&EventEmitter::valueChanged);
    Event::Id tid = EventIdGenerator::typeEventId<EventEmitter>();

    Event::Id stid, seid, sieid;
    EventIdGenerator::separate(eid, stid, seid, sieid);

    QCOMPARE(tid, stid);
}

void EventTest::simpleEventEmissionTest(){
    EventEmitter e(nullptr);
    EventObserver o(nullptr);

    e.on(&EventEmitter::valueChanged, &o, &EventObserver::observeValue);

    e.setValue(20.0);
    QCOMPARE(o.value(), 20.0);
}

void EventTest::eventEmissionThroughLambdaTest(){
    EventEmitter e(nullptr);

    double value = 0;
    e.on(&EventEmitter::valueChanged, [&value](double v){
        value = v;
    });

    e.setValue(20.0);
    QCOMPARE(value, 20.0);

    e.setValue(200.0);
    QCOMPARE(value, 200.0);
}

void EventTest::removeEventFromWithinTest(){
    EventEmitter* e = new EventEmitter(nullptr);

    EventConnection** eHolder = new EventConnection*;

    double value = 0;

    *eHolder = e->on(&EventEmitter::valueChanged, [eHolder, &value, e](double v){
        value = v;
        EventConnection::remove(*eHolder);
        e->setValue(40.0);
    });

    e->setValue(20.0);
    QCOMPARE(value, 20.0);

    delete eHolder;
    delete e;
}

void EventTest::removeAllEventsFromWithinTest(){
    EventEmitter e(nullptr);

    EventConnection** eHolder = new EventConnection*;

    double value = 0;

    *eHolder = e.on(&EventEmitter::valueChanged, [&value, &e](double v){
        value = v;
        e.removeListeners();
        e.setValue(40.0);
    });

    e.setValue(20.0);
    QCOMPARE(value, 20.0);
}
