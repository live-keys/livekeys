#include "jseventtest.h"
#include "live/elements/event.h"
#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/module.h"

Q_TEST_RUNNER_REGISTER(JsEventTest);

using namespace lv;
using namespace lv::el;


class JsEventStub : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(JsEventStub)
            .base<Element>()
            .constructor<>()
            .scriptEvent("pinged", &JsEventStub::pinged)
        META_OBJECT_CLOSE
    }

public:
    JsEventStub(Engine* engine) : Element(engine){}

    Event pinged(int number){ return notify(eventId(&JsEventStub::pinged), number); }
};

class JsEventTypes : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(JsEventTypes)
            .base<Element>()
            .constructor<>()
            .scriptEvent("boolEvent", &JsEventTypes::boolEvent)
            .scriptEvent("intEvent", &JsEventTypes::intEvent)
            .scriptEvent("int64Event", &JsEventTypes::int64Event)
            .scriptEvent("numberEvent", &JsEventTypes::numberEvent)
            .scriptEvent("stdStringEvent", &JsEventTypes::stdStringEvent)
            .scriptEvent("callableEvent", &JsEventTypes::callableEvent)
            .scriptEvent("objectEvent", &JsEventTypes::objectEvent)
            .scriptEvent("localValueEvent", &JsEventTypes::localValueEvent)
            .scriptEvent("valueEvent", &JsEventTypes::valueEvent)
            .scriptEvent("bufferEvent", &JsEventTypes::bufferEvent)
            .scriptEvent("elementEvent", &JsEventTypes::elementEvent)
            .scriptEvent("userElementEvent", &JsEventTypes::userElementEvent)
        META_OBJECT_CLOSE
    }

public:
    JsEventTypes(Engine* engine) : Element(engine){}

    Event boolEvent(bool value){
        static Event::Id eid = eventId(&JsEventTypes::boolEvent);
        return notify(eid, value);
    }

    Event intEvent(int number){
        static Event::Id eid = eventId(&JsEventTypes::intEvent);
        return notify(eid, number);
    }
    Event int64Event(Value::Int64 number){
        static Event::Id eid = eventId(&JsEventTypes::int64Event);
        return notify(eid, number);
    }
    Event numberEvent(Value::Number number){
        static Event::Id eid = eventId(&JsEventTypes::numberEvent);
        return notify(eid, number);
    }
    Event stdStringEvent(const std::string& value){
        static Event::Id eid = eventId(&JsEventTypes::stdStringEvent);
        return notify(eid, value);
    }
    Event callableEvent(Callable value){
        static Event::Id eid = eventId(&JsEventTypes::callableEvent);
        return notify(eid, value);
    }
    Event objectEvent(Object value){
        static Event::Id eid = eventId(&JsEventTypes::objectEvent);
        return notify(eid, value);
    }
    Event localValueEvent(LocalValue value){
        static Event::Id eid = eventId(&JsEventTypes::localValueEvent);
        return notify(eid, value);
    }
    Event valueEvent(Value value){
        static Event::Id eid = eventId(&JsEventTypes::valueEvent);
        return notify(eid, value);
    }
    Event bufferEvent(Buffer value){
        static Event::Id eid = eventId(&JsEventTypes::bufferEvent);
        return notify(eid, value);
    }
    Event elementEvent(Element* value){
        static Event::Id eid = eventId(&JsEventTypes::elementEvent);
        return notify(eid, value);
    }
    Event userElementEvent(JsEventStub* value){
        static Event::Id eid = eventId(&JsEventTypes::userElementEvent);
        return notify(eid, value);
    }

};


class JsEventTypesInheritLevel1Stub : public JsEventTypes{

    META_OBJECT{
        META_OBJECT_DESCRIBE(JsEventTypesInheritLevel1Stub)
            .base<JsEventTypes>()
            .constructor<>()
            .scriptEvent("derivedEvent", &JsEventTypesInheritLevel1Stub::derivedEvent)
        META_OBJECT_CLOSE
    }

public:
    JsEventTypesInheritLevel1Stub(lv::el::Engine* engine) : JsEventTypes(engine){}

    Event derivedEvent(){
        static Event::Id eid = eventId(&JsEventTypesInheritLevel1Stub::derivedEvent);
        return notify(eid);
    }
};

class JsEventTypesInheritLevel2Stub : public JsEventTypesInheritLevel1Stub{

    META_OBJECT{
        META_OBJECT_DESCRIBE(JsEventTypesInheritLevel2Stub)
            .base<JsEventTypesInheritLevel1Stub>()
            .constructor<>()
            .scriptEvent("intEvent", &JsEventTypesInheritLevel2Stub::intEvent2)
        META_OBJECT_CLOSE
    }

public:
    JsEventTypesInheritLevel2Stub(lv::el::Engine* engine)
        : JsEventTypesInheritLevel1Stub(engine)
    {}

public:
    Event intEvent2(Value::Int32 value){
        static Event::Id eid = eventId(&JsEventTypesInheritLevel2Stub::intEvent2);
        return notify(eid, value);
    }

};

class EventPropertyStub : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(EventPropertyStub)
            .base<Element>()
            .constructor()
            .scriptProperty<Value::Int32>("eventProperty",
                                          &EventPropertyStub::eventProperty,
                                          &EventPropertyStub::setEventProperty,
                                          "eventPropertyChanged")
            .scriptEvent("eventPropertyChanged", &EventPropertyStub::eventPropertyChanged)
        META_OBJECT_CLOSE
    }

    EventPropertyStub(Engine* engine) : Element(engine), m_eventProperty(0){}

public:
    Value::Int32 eventProperty() const{ return m_eventProperty; }
    void setEventProperty(Value::Int32 value){
        if ( m_eventProperty == value )
            return;

        m_eventProperty = value;
        eventPropertyChanged();
    }

    Event eventPropertyChanged(){
        static Event::Id eid = eventId(&EventPropertyStub::eventPropertyChanged);
        return notify(eid);
    }

private:
    int m_eventProperty;
};


JsEventTest::JsEventTest(QObject *parent)
    : QObject(parent)
{
}

void JsEventTest::initTestCase(){
}

void JsEventTest::callEventTest(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){
            JsEventStub* jsEvent = new JsEventStub(engine);
            jsEvent->ref();

            int pingNumber = 0;

            jsEvent->on(&JsEventStub::pinged, [&pingNumber](int number){
                pingNumber = number;
            });

            LocalObject globalObject(engine->currentContext());
            globalObject.set(engine, "jsEvent", LocalValue(engine, jsEvent));
            engine->compileEnclosed("jsEvent.pinged(2000);")->run();

            QCOMPARE(pingNumber, 2000);

            delete jsEvent;
        });
    }

    delete engine;
}

void JsEventTest::registerListenerTest()
{
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){

            JsEventStub* jsEvent = new JsEventStub(engine);
            jsEvent->ref();

            LocalObject globalObject(engine->currentContext());
            globalObject.set(engine, "jsEvent", LocalValue(engine, jsEvent));
            globalObject.set(engine, "jsEventLastPing", LocalValue(engine, 0));

            engine->compileEnclosed("jsEvent.on('pinged', function(number){ jsEventLastPing = number; });")->run();

            jsEvent->pinged(2000);
            int pingNumber = globalObject.get(engine, "jsEventLastPing").toInt32(engine);

            QCOMPARE(pingNumber, 2000);

            delete jsEvent;
        });

    }

    delete engine;
}

void JsEventTest::registerListenerThroughMetaTest(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){

            JsEventStub* jsEvent = new JsEventStub(engine);
            jsEvent->ref();

            int pingNumber = 0;
            jsEvent->on("pinged", [&pingNumber, engine](const Function::Parameters& p){
                pingNumber = p.at(engine, 0).toInt32(engine);
            });
            jsEvent->pinged(2000);;
            QCOMPARE(pingNumber, 2000);

            delete jsEvent;
        });
    }

    delete engine;
}

void JsEventTest::triggerEventThroughMetaTest(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){

            JsEventStub* jsEvent = new JsEventStub(engine);
            jsEvent->ref();

            LocalObject globalObject(engine->currentContext());
            globalObject.set(engine, "jsEvent", LocalValue(engine, jsEvent));
            globalObject.set(engine, "jsEventLastPing", LocalValue(engine, 0));

            engine->compileEnclosed("jsEvent.on('pinged', function(number){ jsEventLastPing = number; });")->run();

            Function::Parameters p(1);
            p.assign(0, LocalValue(engine, 2000));
            jsEvent->trigger("pinged", p);

            int pingNumber = globalObject.get(engine, "jsEventLastPing").toInt32(engine);

            QCOMPARE(pingNumber, 2000);

            delete jsEvent;
        });
    }

    delete engine;
}

void JsEventTest::addEventJsCallInJsTest()
{
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){
            JsEventStub* jsEvent = new JsEventStub(engine);
            jsEvent->ref();

            LocalObject globalObject(engine->currentContext());
            globalObject.set(engine, "jsEvent", LocalValue(engine, jsEvent));

            engine->compileEnclosed("Element.addEvent(jsEvent, 'customEvent', ['int']);")->run();

            int eventValue = 0;
            jsEvent->on("customEvent", [&engine, &eventValue](const Function::Parameters& p){
                eventValue = p.at(engine, 0).toInt32(engine);
            });

            engine->compileEnclosed("jsEvent.customEvent(200);")->run();

            QCOMPARE(eventValue, 200);

            delete jsEvent;
        });
    }

    delete engine;
}

void JsEventTest::addEventListenInJsTest(){

    Engine* engine = new Engine();
    {
        engine->scope([&engine](){
            JsEventStub* jsEvent = new JsEventStub(engine);
            jsEvent->ref();

            LocalObject globalObject(engine->currentContext());
            globalObject.set(engine, "jsEvent", LocalValue(engine, jsEvent));
            globalObject.set(engine, "jsEventLastValue", LocalValue(engine, 0));

            std::vector<std::string> eventTypes = {"int"};
            Element::addEvent(jsEvent, "customEvent", eventTypes);

            engine->compileEnclosed("jsEvent.on('customEvent', function(number){ jsEventLastValue = number; });")->run();

            Function::Parameters p(1);
            p.assign(0, LocalValue(engine, 2000));

            jsEvent->trigger("customEvent", p);
            int pingNumber = globalObject.get(engine, "jsEventLastValue").toInt32(engine);

            QCOMPARE(pingNumber, 2000);

            delete jsEvent;
        });
    }

    delete engine;
}

void JsEventTest::eventTypesTest(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){
            JsEventTypes* jsEvent = new JsEventTypes(engine);
            jsEvent->ref();

            LocalObject globalObject(engine->currentContext());
            globalObject.set(engine, "jsEvent", LocalValue(engine, jsEvent));

            bool boolEventValue = false;
            jsEvent->on("boolEvent", [&boolEventValue, engine](const Function::Parameters& p){
                boolEventValue = p.at(engine, 0).toBool(engine);
            });
            engine->compileEnclosed("jsEvent.boolEvent(true);")->run();
            QCOMPARE(boolEventValue, true);

            Value::Int32 intEventValue = 0;
            jsEvent->on("intEvent", [&intEventValue, engine](const Function::Parameters& p){
                intEventValue = p.at(engine, 0).toInt32(engine);
            });
            engine->compileEnclosed("jsEvent.intEvent(200);")->run();
            QCOMPARE(intEventValue, 200);

            Value::Int64 int64EventValue = 0;
            jsEvent->on("int64Event", [&int64EventValue, engine](const Function::Parameters& p){
                int64EventValue = p.at(engine, 0).toInt64(engine);
            });
            engine->compileEnclosed("jsEvent.int64Event(300);")->run();
            QCOMPARE(int64EventValue, 300);

            Value::Number numberEventValue = 0;
            jsEvent->on("numberEvent", [&numberEventValue, engine](const Function::Parameters& p){
                numberEventValue = p.at(engine, 0).toNumber(engine);
            });
            engine->compileEnclosed("jsEvent.numberEvent(300.300);")->run();
            QCOMPARE(numberEventValue, 300.300);

            std::string stdStringEventValue;
            jsEvent->on("stdStringEvent", [&stdStringEventValue, engine](const Function::Parameters& p){
                stdStringEventValue = p.at(engine, 0).toStdString(engine);
            });
            engine->compileEnclosed("jsEvent.stdStringEvent('asd');")->run();
            QVERIFY(stdStringEventValue == "asd");

            Callable callableEventValue(engine);
            QVERIFY(callableEventValue.isNull());
            jsEvent->on("callableEvent", [&callableEventValue, engine](const Function::Parameters& p){
                callableEventValue = p.at(engine, 0).toCallable(engine);
            });
            engine->compileEnclosed("jsEvent.callableEvent(function(){ return 20; });")->run();
            QVERIFY(!callableEventValue.isNull());
            QVERIFY(callableEventValue.call(engine, Function::Parameters(0)).toInt32(engine) == 20);

            Buffer bufferEventValue(nullptr, 0);
            jsEvent->on("bufferEvent", [&bufferEventValue, engine](const Function::Parameters& p){
                bufferEventValue = p.at(engine, 0).toBuffer(engine);
            });
            engine->compileEnclosed("jsEvent.bufferEvent(new ArrayBuffer(8));")->run();
            QVERIFY(bufferEventValue.size() == 8);

            Object objectEventValue(engine);
            QVERIFY(objectEventValue.isNull());
            jsEvent->on("objectEvent", [&objectEventValue, engine](const Function::Parameters& p){
                objectEventValue = p.at(engine, 0).toObject(engine);
            });
            engine->compileEnclosed("jsEvent.objectEvent({a : 1, b: 20.2});")->run();
            QVERIFY(!objectEventValue.isNull());
            LocalObject localObjectEventValue(objectEventValue);
            QVERIFY(localObjectEventValue.get(engine, std::string("a")).toInt32(engine) == 1);
            QVERIFY(localObjectEventValue.get(engine, std::string("b")).toNumber(engine) == 20.2);

            LocalValue localValueEventValue(engine);
            jsEvent->on("localValueEvent", [&localValueEventValue, engine](const Function::Parameters& p){
                localValueEventValue = p.at(engine, 0);
            });
            engine->compileEnclosed("jsEvent.localValueEvent('asd');")->run();
            QVERIFY(localValueEventValue.toStdString(engine) == "asd");

            engine->compileEnclosed("jsEvent.localValueEvent(200);")->run();
            QVERIFY(localValueEventValue.toInt32(engine) == 200);

            Value valueEventValue;
            jsEvent->on("valueEvent", [&valueEventValue, engine](const Function::Parameters& p){
                valueEventValue = p.at(engine, 0).toValue(engine);
            });
            engine->compileEnclosed("jsEvent.valueEvent({a: 1, b: 20.2});")->run();
            Object valueEventObject = valueEventValue.asObject();
            QVERIFY(!valueEventObject.isNull());
            LocalObject valueEventLocalObject(valueEventObject);
            QVERIFY(valueEventLocalObject.get(engine, std::string("a")).toInt32(engine) == 1);
            QVERIFY(valueEventLocalObject.get(engine, std::string("b")).toNumber(engine) == 20.2);

            Element* triggeredElement = new Element(engine);
            triggeredElement->ref();
            Element* elementEventValue = nullptr;
            globalObject.set(engine, "triggeredElement", LocalValue(engine, triggeredElement));
            jsEvent->on("elementEvent", [&elementEventValue, engine](const Function::Parameters& p){
                elementEventValue = p.at(engine, 0).toElement(engine);
            });
            engine->compileEnclosed("jsEvent.elementEvent(triggeredElement);")->run();
            QVERIFY(elementEventValue == triggeredElement);

            Element* userElementEventValue = nullptr;
            jsEvent->on("userElementEvent", [&userElementEventValue, engine](const Function::Parameters& p){
                userElementEventValue = p.at(engine, 0).toElement(engine);
            });
            engine->compileEnclosed("jsEvent.userElementEvent(jsEvent);")->run();
            QVERIFY(userElementEventValue == jsEvent);

            delete triggeredElement;
            delete jsEvent;
        });
    }

    delete engine;
}

void JsEventTest::eventInheritanceTest(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){

            JsEventTypesInheritLevel2Stub* jsEvent = new JsEventTypesInheritLevel2Stub(engine);
            jsEvent->ref();

            LocalObject globalObject(engine->currentContext());
            globalObject.set(engine, "jsEvent", LocalValue(engine, jsEvent));


            Value::Int32 intEventValue = 0;
            jsEvent->on("intEvent", [&intEventValue, engine](const Function::Parameters& p){
                intEventValue = p.at(engine, 0).toInt32(engine);
            });
            engine->compileEnclosed("jsEvent.intEvent(200);")->run();
            QCOMPARE(intEventValue, 200);

            jsEvent->intEvent2(400);
            QCOMPARE(intEventValue, 400);

            Value::Int64 int64EventValue = 0;
            jsEvent->on("int64Event", [&int64EventValue, engine](const Function::Parameters& p){
                int64EventValue = p.at(engine, 0).toInt64(engine);
            });
            engine->compileEnclosed("jsEvent.int64Event(300);")->run();
            QCOMPARE(int64EventValue, 300);

            delete jsEvent;
        });
    }

    delete engine;
}

void JsEventTest::eventOnProperty(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){
            EventPropertyStub* e = new EventPropertyStub(engine);
            e->ref();

            int pValue = 0;
            e->on("eventPropertyChanged", [&pValue, e](const Function::Parameters&){
                pValue = e->get("eventProperty").toInt32(e->engine());
            });
            e->set("eventProperty", LocalValue(engine, 300));

            QCOMPARE(pValue, 300);

            delete e;
        });
    }

    delete engine;
}

void JsEventTest::eventOnCustomProperty(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){
            Element* e = new Element(engine);
            e->ref();
            e->addProperty("p", "int", LocalValue(engine, 200), false, true, "pChanged");
            QVERIFY(e->hasProperty("p"));

            int pValue = 0;
            e->on("pChanged", [&pValue, e](const Function::Parameters&){
                pValue = e->get("p").toInt32(e->engine());
            });
            e->set("p", LocalValue(engine, 300));

            QCOMPARE(pValue, 300);

            delete e;
        });
    }

    delete engine;
}
