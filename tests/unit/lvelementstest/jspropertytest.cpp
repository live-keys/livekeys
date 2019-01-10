#include "jspropertytest.h"
#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/component.h"
#include "live/elements/modulelibrary.h"

Q_TEST_RUNNER_REGISTER(JsPropertyTest);

using namespace lv;
using namespace lv::el;

class ReadOnlyPropertyStub : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(ReadOnlyPropertyStub)
            .base<Element>()
            .constructor<>()
            .scriptProperty<int>("readOnlyProperty", &ReadOnlyPropertyStub::readOnlyProperty)
        META_OBJECT_CLOSE
    }

    ReadOnlyPropertyStub(lv::el::Engine* engine) : Element(engine), m_readOnlyProperty(200){}

    int readOnlyProperty() const{ return m_readOnlyProperty; }

private:
    int m_readOnlyProperty;
};


class PropertyTypesStub : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(PropertyTypesStub)
            .base<Element>()
            .constructor<>()
            .scriptProperty<bool>("boolProperty",
                                  &PropertyTypesStub::boolProperty,
                                  &PropertyTypesStub::setBoolProperty,
                                  "boolPropertyChanged")
            .scriptProperty<Value::Int32>("intProperty",
                                          &PropertyTypesStub::intProperty,
                                          &PropertyTypesStub::setIntProperty,
                                          "intPropertyChanged")

            .scriptProperty<Value::Int64>("int64Property",
                                          &PropertyTypesStub::int64Property,
                                          &PropertyTypesStub::setInt64Property,
                                          "int64PropertyChanged")

            .scriptProperty<Value::Number>("numberProperty",
                                           &PropertyTypesStub::numberProperty,
                                           &PropertyTypesStub::setNumberProperty,
                                           "numberPropertyChanged")

            .scriptProperty<std::string>("stdStringProperty",
                                         &PropertyTypesStub::stdStringProperty,
                                         &PropertyTypesStub::setStdStringProperty,
                                         "stdStringPropertyChanged")

            .scriptProperty<lv::el::Callable>("callableProperty",
                                              &PropertyTypesStub::callableProperty,
                                              &PropertyTypesStub::setCallableProperty,
                                              "callablePropertyChanged")

            .scriptProperty<lv::el::Object>("objectProperty",
                                            &PropertyTypesStub::objectProperty,
                                            &PropertyTypesStub::setObjectProperty,
                                            "callablePropertyChanged")

            .scriptProperty<lv::el::LocalValue>("localValueProperty",
                                                &PropertyTypesStub::localValueProperty,
                                                &PropertyTypesStub::setLocalValueProperty,
                                                "localValuePropertyChanged")

            .scriptProperty<lv::el::Value>("valueProperty",
                                           &PropertyTypesStub::valueProperty,
                                           &PropertyTypesStub::setValueProperty,
                                           "valuePropertyChanged")

            .scriptProperty<lv::el::Buffer>("bufferProperty",
                                            &PropertyTypesStub::bufferProperty,
                                            &PropertyTypesStub::setBufferProperty,
                                            "bufferPropertyChanged")

            .scriptProperty<lv::el::Element*>("elementProperty",
                                              &PropertyTypesStub::elementProperty,
                                              &PropertyTypesStub::setElementProperty,
                                              "elementPropertyChanged")

            .scriptProperty<ReadOnlyPropertyStub*>("userElementProperty",
                                                   &PropertyTypesStub::userElementProperty,
                                                   &PropertyTypesStub::setUserElementProperty,
                                                   "userElementPropertyChanged")

            .scriptEvent("intPropertyChanged", &PropertyTypesStub::intPropertyChanged)
            .scriptEvent("int64PropertyChanged", &PropertyTypesStub::int64PropertyChanged)
            .scriptEvent("numberPropertyChanged", &PropertyTypesStub::numberPropertyChanged)
            .scriptEvent("stdStringPropertyChanged", &PropertyTypesStub::stdStringPropertyChanged)
            .scriptEvent("callablePropertyChanged", &PropertyTypesStub::callablePropertyChanged)
            .scriptEvent("objectPropertyChanged", &PropertyTypesStub::objectPropertyChanged)
            .scriptEvent("localValuePropertyChanged", &PropertyTypesStub::localValuePropertyChanged)
            .scriptEvent("valuePropertyChanged", &PropertyTypesStub::valuePropertyChanged)
            .scriptEvent("bufferPropertyChanged", &PropertyTypesStub::bufferPropertyChanged)
            .scriptEvent("elementPropertyChanged", &PropertyTypesStub::elementPropertyChanged)
            .scriptEvent("userElementPropertyChanged", &PropertyTypesStub::userElementPropertyChanged)

        META_OBJECT_CLOSE
    }

    PropertyTypesStub(lv::el::Engine* engine)
        : Element(engine)
        , m_boolProperty(false)
        , m_intProperty(0)
        , m_int64Property(0)
        , m_numberProperty(0)
        , m_callableProperty(engine)
        , m_objectProperty(engine)
        , m_bufferProperty(0)
        , m_bufferPropertySize(0)
        , m_elementProperty(nullptr)
        , m_userElementProperty(nullptr)
        , m_localValueProperty()
    {
    }

    bool boolProperty() const{ return m_boolProperty; }
    void setBoolProperty(bool value){
        if ( value == m_boolProperty )
            return;

        m_boolProperty = value;
        boolPropertyChanged();
    }


    Value::Int32 intProperty() const{ return m_intProperty; }
    void setIntProperty(Value::Int32 value){
        if ( value == m_intProperty )
            return;

        m_intProperty = value;
        intPropertyChanged();
    }

    Value::Int64 int64Property() const{ return m_int64Property; }
    void setInt64Property(Value::Int64 value){
        if ( value == m_int64Property )
            return;

        m_int64Property = value;
        int64PropertyChanged();
    }

    Value::Number numberProperty() const{ return m_numberProperty; }
    void setNumberProperty(Value::Number value){
        if ( value == m_numberProperty )
            return;

        m_numberProperty = value;
        numberPropertyChanged();
    }

    const std::string& stdStringProperty() const{ return m_stdStringProperty; }
    void setStdStringProperty(const std::string& value){
        if ( value == m_stdStringProperty )
            return;

        m_stdStringProperty = value;
        stdStringPropertyChanged();
    }

    Callable callableProperty() const{ return m_callableProperty; }
    void setCallableProperty(Callable value){
        if ( value == m_callableProperty )
            return;

        m_callableProperty = value;
        callablePropertyChanged();
    }

    Object objectProperty() const{ return m_objectProperty; }
    void setObjectProperty(Object value){
        if ( value == m_objectProperty )
            return;

        m_objectProperty = value;
        objectPropertyChanged();
    }

    lv::el::LocalValue localValueProperty() const{ return lv::el::LocalValue(engine(), m_localValueProperty); }
    void setLocalValueProperty(lv::el::LocalValue value){
        lv::el::Value v = value.toValue(engine());

        if ( m_localValueProperty == v )
            return;

        m_localValueProperty = v;
        localValuePropertyChanged();
    }

    lv::el::Value valueProperty() const{ return m_valueProperty; }
    void setValueProperty(lv::el::Value value){
        if ( m_valueProperty == value )
            return;

        m_valueProperty = value;
        valuePropertyChanged();
    }

    Buffer bufferProperty() const{ return Buffer(m_bufferProperty, m_bufferPropertySize); }
    void setBufferProperty(Buffer value){
        if ( value.dataAs<int*>() == m_bufferProperty )
            return;

        m_bufferProperty = value.dataAs<int*>();
        m_bufferPropertySize  = value.size();
        bufferPropertyChanged();
    }

    Element* elementProperty(){ return m_elementProperty; }
    void setElementProperty(Element* value){
        if ( m_elementProperty == value )
            return;

        m_elementProperty = value;
        elementPropertyChanged();
    }

    ReadOnlyPropertyStub* userElementProperty(){ return m_userElementProperty; }
    void setUserElementProperty(ReadOnlyPropertyStub* value){
        if ( m_userElementProperty == value )
            return;

        m_userElementProperty = value;
        userElementPropertyChanged();
    }

public:
    Event boolPropertyChanged(){ static Event::Id eid = eventId(&PropertyTypesStub::boolPropertyChanged); return notify(eid);}
    Event intPropertyChanged(){ static Event::Id eid = eventId(&PropertyTypesStub::intPropertyChanged); return notify(eid); }
    Event int64PropertyChanged(){ static Event::Id eid = eventId(&PropertyTypesStub::int64PropertyChanged); return notify(eid); }
    Event numberPropertyChanged(){ static Event::Id eid = eventId(&PropertyTypesStub::numberPropertyChanged); return notify(eid); }
    Event stdStringPropertyChanged(){ static Event::Id eid = eventId(&PropertyTypesStub::stdStringPropertyChanged); return notify(eid); }
    Event callablePropertyChanged(){ static Event::Id eid = eventId(&PropertyTypesStub::callablePropertyChanged); return notify(eid); }
    Event objectPropertyChanged(){ static Event::Id eid = eventId(&PropertyTypesStub::objectPropertyChanged); return notify(eid); }
    Event localValuePropertyChanged(){ static Event::Id eid = eventId(&PropertyTypesStub::localValuePropertyChanged); return notify(eid); }
    Event valuePropertyChanged(){ static Event::Id eid = eventId(&PropertyTypesStub::valuePropertyChanged); return notify(eid); }
    Event bufferPropertyChanged(){ static Event::Id eid = eventId(&PropertyTypesStub::bufferPropertyChanged); return notify(eid); }
    Event elementPropertyChanged(){ static Event::Id eid = eventId(&PropertyTypesStub::elementPropertyChanged); return notify(eid); }
    Event userElementPropertyChanged(){ static Event::Id eid = eventId(&PropertyTypesStub::userElementPropertyChanged); return notify(eid); }

private:
    bool                  m_boolProperty;
    Value::Int32          m_intProperty;
    Value::Int64          m_int64Property;
    Value::Number         m_numberProperty;
    std::string           m_stdStringProperty;
    lv::el::Callable      m_callableProperty;
    lv::el::Object        m_objectProperty;
    int*                  m_bufferProperty;
    size_t                m_bufferPropertySize;
    lv::el::Element*      m_elementProperty;
    ReadOnlyPropertyStub* m_userElementProperty;
    lv::el::Value         m_localValueProperty;
    lv::el::Value         m_valueProperty;
};

class PropertyTypesInheritLevel1Stub : public PropertyTypesStub{

    META_OBJECT{
        META_OBJECT_DESCRIBE(PropertyTypesInheritLevel1Stub)
            .base<PropertyTypesStub>()
            .constructor<>()
            .scriptProperty<int>("derivedProperty", &PropertyTypesInheritLevel1Stub::derivedProperty)
        META_OBJECT_CLOSE
    }

public:
    PropertyTypesInheritLevel1Stub(lv::el::Engine* engine) : PropertyTypesStub(engine), m_derivedProperty(200){}

    int derivedProperty() const{ return m_derivedProperty; }

private:
    int m_derivedProperty;
};

class PropertyTypesInheritLevel2Stub : public PropertyTypesInheritLevel1Stub{

    META_OBJECT{
        META_OBJECT_DESCRIBE(PropertyTypesInheritLevel2Stub)
            .base<PropertyTypesInheritLevel1Stub>()
            .constructor<>()
            .scriptProperty<int>("intProperty",
                                 &PropertyTypesInheritLevel2Stub::intOverrideProperty,
                                 &PropertyTypesInheritLevel2Stub::setIntOverrideProperty,
                                 "intOverrideChanged")
        META_OBJECT_CLOSE
    }

public:
    PropertyTypesInheritLevel2Stub(lv::el::Engine* engine)
        : PropertyTypesInheritLevel1Stub(engine)
        , m_intOverrideProperty(0)
    {}

    int intOverrideProperty() const{ return m_intOverrideProperty; }
    void setIntOverrideProperty(int value){
        if ( value == m_intOverrideProperty )
            return;

        m_intOverrideProperty = value;
        intOverrideChanged();
    }

public:
    Event intOverrideChanged(){ static Event::Id eid = eventId(&PropertyTypesInheritLevel2Stub::intOverrideChanged); return notify(eid);}

private:
    int m_intOverrideProperty;
};


JsPropertyTest::JsPropertyTest(QObject *parent)
    : QObject(parent)
{
}

void JsPropertyTest::initTestCase(){

}

void JsPropertyTest::readOnlyProperty(){
    Engine* engine = new Engine();
    {
        engine->scope([engine](){
            ReadOnlyPropertyStub* e = new ReadOnlyPropertyStub(engine);
            e->ref();

            QCOMPARE(e->get("readOnlyProperty").toInt32(engine), 200);
            delete e;
        });
    }

    delete engine;
}

void JsPropertyTest::propertyTypes(){
    Engine* engine = new Engine();
    {
        engine->scope([engine](){
            PropertyTypesStub* e = new PropertyTypesStub(engine);
            e->ref();

            e->setBoolProperty(true);
            QCOMPARE(e->boolProperty(), true);
            QCOMPARE(e->get("boolProperty").toBool(engine), true);

            e->set("boolProperty", LocalValue(engine, false));
            QCOMPARE(e->boolProperty(), false);
            QCOMPARE(e->get("boolProperty").toBool(engine), false);

            e->setIntProperty(20);
            QCOMPARE(e->intProperty(), 20);
            QCOMPARE(e->get("intProperty").toInt32(engine), 20);

            e->set("intProperty", LocalValue(engine, 40));
            QCOMPARE(e->intProperty(), 40);
            QCOMPARE(e->get("intProperty").toInt32(engine), 40);

            e->setInt64Property(30);
            QCOMPARE(e->int64Property(), 30);
            QCOMPARE(e->get("int64Property").toInt64(engine), 30);

            e->set("int64Property", LocalValue(engine, 50));
            QCOMPARE(e->int64Property(), 50);
            QCOMPARE(e->get("int64Property").toInt32(engine), 50);

            double v = 30.3;
            e->setNumberProperty(v);
            QCOMPARE(e->numberProperty(), v);
            QCOMPARE(e->get("numberProperty").toNumber(engine), v);

            v = 50.3;
            e->set("numberProperty", LocalValue(engine, v));
            QCOMPARE(e->numberProperty(), v);
            QCOMPARE(e->get("numberProperty").toNumber(engine), v);

            std::string s = "123";
            e->setStdStringProperty(s);
            QCOMPARE(e->stdStringProperty(), s);
            QCOMPARE(e->get("stdStringProperty").toStdString(engine), s);

            s = "987";
            e->set("stdStringProperty", LocalValue(engine, s));
            QCOMPARE(e->stdStringProperty(), s);
            QCOMPARE(e->get("stdStringProperty").toStdString(engine), s);

            Callable callable = engine->compileEnclosed("return function(){ return 20; }")->run().asCallable();
            QVERIFY(!callable.isNull());
            e->set("callableProperty", LocalValue(engine, callable));
            QVERIFY(e->callableProperty().call(engine, Function::Parameters(0)).toInt32(engine) == 20);
            QVERIFY(e->get("callableProperty").toCallable(engine) == callable);

            Object o = engine->compileEnclosed("return { a : 2 };")->run().asObject();
            QVERIFY(!o.isNull());
            e->set("objectProperty", LocalValue(engine, o));
            LocalObject lo(e->objectProperty());
            QCOMPARE(lo.get(LocalValue(engine, std::string("a"))).toInt32(engine), 2);
            QVERIFY(e->objectProperty() == o);

            e->set("localValueProperty", LocalValue(engine, 200.0));
            QCOMPARE(e->localValueProperty().toNumber(engine), 200.0);
            QCOMPARE(e->get("localValueProperty").toNumber(engine), 200.0);
            e->set("localValueProperty", LocalValue(engine, std::string("abc")));
            QCOMPARE(e->localValueProperty().toStdString(engine), std::string("abc"));
            QCOMPARE(e->get("localValueProperty").toStdString(engine), std::string("abc"));

            e->set("valueProperty", LocalValue(engine, 100.0));
            QCOMPARE(e->valueProperty().asNumber(), 100.0);
            QCOMPARE(e->get("valueProperty").toValue(engine).asNumber(), 100.0);
            e->set("valueProperty", LocalValue(engine, callable));
            QVERIFY(e->valueProperty().asCallable() == callable);
            QVERIFY(e->get("valueProperty").toValue(engine).asCallable() == callable);

            int* buffer = new int[5];
            for ( int i = 0; i < 5; ++i )
                buffer[i] = i;
            size_t bufferSize = 5;
            e->setBufferProperty(Buffer(buffer, bufferSize));
            QVERIFY(e->bufferProperty().size() == 5);
            QVERIFY(e->bufferProperty().data() == buffer);
            QVERIFY(e->get("bufferProperty").toBuffer(engine).data() == buffer);
            QVERIFY(e->get("bufferProperty").toBuffer(engine).size() == bufferSize);
            QVERIFY(e->get("bufferProperty").toBuffer(engine).isExternal());

            e->set("bufferProperty", LocalValue(engine, Buffer(nullptr, 0)));
            QVERIFY(e->bufferProperty().size() == 0);
            QVERIFY(e->bufferProperty().data() == nullptr);
            QVERIFY(e->get("bufferProperty").toBuffer(engine).size() == 0);
            delete []buffer;

            Element* elemProperty = new Element(engine);
            e->set("elementProperty", LocalValue(engine, elemProperty));
            QVERIFY(e->elementProperty() == elemProperty);
            QVERIFY(e->get("elementProperty").toElement(engine) == elemProperty);
            e->set("elementProperty", LocalValue(engine));
            QVERIFY(e->elementProperty() == nullptr);
            delete elemProperty;

            ReadOnlyPropertyStub* userElemProperty = new ReadOnlyPropertyStub(engine);
            LocalValue vvv(engine, userElemProperty);

            e->set("userElementProperty", vvv);
            QVERIFY(e->get("userElementProperty").toElement(engine) == userElemProperty);
            e->set("userElementProperty", LocalValue(engine));
            QVERIFY(e->userElementProperty() == nullptr);
            delete userElemProperty;

            Element* ps = new ReadOnlyPropertyStub(engine);

            LocalObject globalObject(engine->currentContext());
            globalObject.set(engine, "e", LocalValue(engine, e));
            globalObject.set(engine, "ps", LocalValue(engine, ps));
            engine->compileEnclosed(
                "e.boolProperty = true; "
                "e.intProperty = 7; "
                "e.int64Property = 77;"
                "e.numberProperty = 70; "
                "e.stdStringProperty = 'abcd';"
                "e.callableProperty = function(){ return 700; };"
                "e.objectProperty = {b: 7};"
                "e.bufferProperty = new ArrayBuffer(8); "
                "e.localValueProperty = 'abcd';"
                "e.valueProperty = new String('abcd');"
                "e.elementProperty = e;"
                "e.userElementProperty = ps;"
                "return e;"
            )->run();

            QVERIFY(e->boolProperty() == true);
            QVERIFY(e->intProperty() == 7);
            QVERIFY(e->int64Property() == 77);
            QVERIFY(e->numberProperty() == 70);
            QVERIFY(e->stdStringProperty() == std::string("abcd"));
            QVERIFY(e->callableProperty().call(engine, Function::Parameters(0)).toInt32(engine) == 700);
            LocalObject lop(e->objectProperty());
            QVERIFY(lop.get(LocalValue(engine, std::string("b"))).toInt32(engine) == 7);
            QVERIFY(e->localValueProperty().toStdString(engine) == "abcd");
            QVERIFY(LocalValue(engine, e->valueProperty()).toStdString(engine) == "abcd");
            QVERIFY(e->bufferProperty().size() == 8);
            QVERIFY(e->elementProperty() == e);
            QVERIFY(e->userElementProperty() == ps);

            delete ps;
            delete e;
        });
    }
    delete engine;
}

void JsPropertyTest::defaultProperty(){

}

void JsPropertyTest::propertyWithExpression(){

}

void JsPropertyTest::propertyInheritance(){
    Engine* engine = new Engine();
    {
        engine->scope([engine](){
            PropertyTypesInheritLevel1Stub* e = new PropertyTypesInheritLevel1Stub(engine);
            e->ref();
            QCOMPARE(e->get("derivedProperty").toInt32(engine), 200);

            e->setIntProperty(10);
            QCOMPARE(e->get("intProperty").toInt32(engine), 10);

            delete e;

            PropertyTypesInheritLevel2Stub* e2 = new PropertyTypesInheritLevel2Stub(engine);
            e2->ref();
            QCOMPARE(e2->get("derivedProperty").toInt32(engine), 200);

            // Set the base int property
            e2->setIntProperty(100);

            // Sset the derived int property
            e2->set("intProperty", LocalValue(engine, 20));
            QCOMPARE(e2->intProperty(), 100);
            QCOMPARE(e2->intOverrideProperty(), 20);
            QCOMPARE(e2->get("intProperty").toInt32(engine), 20);

            delete e2;

        });
    }

    delete engine;
}

void JsPropertyTest::dynamicReadOnlyProperty(){
    Engine* engine = new Engine();
    {
        engine->scope([engine](){
            Element* e = new Element(engine);
            e->ref();
            e->addProperty("p", "int", LocalValue(engine, 200), false, false, "");
            QVERIFY(e->hasProperty("p"));
            QCOMPARE(e->get("p").toInt32(engine), 200);
            delete e;
        });
    }

    delete engine;
}

void JsPropertyTest::dynamicProperty(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){
            Element* e = new Element(engine);
            e->ref();
            e->addProperty("p", "int", LocalValue(engine, 200), false, true, "pChanged");
            QVERIFY(e->hasProperty("p"));

            e->set("p", LocalValue(engine, 300));
            QCOMPARE(e->get("p").toInt32(engine), 300);

            delete e;
        });
    }

    delete engine;
}

void JsPropertyTest::dynamicPropertyEmptyEvent(){
    //TODO: This should fail with exception
//    Engine* engine = new Engine();
//    {
//        engine->scope([&engine](){
//            Element* e = new Element(engine);
//            e->addProperty("p", "int", LocalValue(engine, 200), false, true, "pChanged");
//            QVERIFY(e->hasProperty("p"));

//            e->set("p", LocalValue(engine, 300));
//            QCOMPARE(e->get("p").toInt32(engine), 300);

//            delete e;
//        });
//    }

//    delete engine;
}

void JsPropertyTest::dynamicPropertyWithEvent(){
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

void JsPropertyTest::dynamicPropertyInvalid(){

}

void JsPropertyTest::dynamicDefaultProperty(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){
            Element* e = new Element(engine);
            e->ref();
            e->addProperty("p", "int", LocalValue(engine, 200), true, true, "pChanged");
            QVERIFY(e->hasProperty("p"));

            QCOMPARE(e->get(e->defaultProperty()).toInt32(engine), 200);

            delete e;
        });
    }

    delete engine;
}

void JsPropertyTest::dynamicDefaultPropertyExists(){
    //TODO: Once exceptions are in place
}

void JsPropertyTest::jsDynamicPropertyWithAssignedExpression(){
    Engine* engine = new Engine();
    {
        engine->scope([engine](){
            Element* e = new Element(engine);
            e->ref();
            e->addProperty("p", "int", LocalValue(engine, 50), false, true, "pChanged");
            QVERIFY(e->hasProperty("p"));

            Element* x = new Element(engine);
            x->addProperty("value", "int", LocalValue(engine, 200), false, true, "valueChanged");
            QVERIFY(x->hasProperty("value"));

            Element* y = new Element(engine);
            y->addProperty("value", "int", LocalValue(engine, 200), false, true, "valueChanged");
            QVERIFY(y->hasProperty("value"));

            LocalObject globalObject(engine->currentContext());
            globalObject.set(engine, "e", LocalValue(engine, e));
            globalObject.set(engine, "x", LocalValue(engine, x));
            globalObject.set(engine, "y", LocalValue(engine, y));

            Script::Ptr s = engine->compile(
                "Element.assignPropertyExpression("
                    "e, 'p', function(){ return x.value + y.value; }, [[x, 'valueChanged'], [y,'valueChanged']]"
                ");"
            );
            s->run();

            QCOMPARE(e->get("p").toInt32(engine), 400);

            x->set("value", LocalValue(engine, 500));
            QCOMPARE(e->get("p").toInt32(engine), 700);

            y->set("value", LocalValue(engine, 400));
            QCOMPARE(e->get("p").toInt32(engine), 900);

            // Reset the binding
            e->set("p", LocalValue(engine, 200));
            QCOMPARE(e->get("p").toInt32(engine), 200);

            // Check binding reset
            x->set("value", LocalValue(engine, 300));
            QCOMPARE(e->get("p").toInt32(engine), 200);

            delete e;
        });
    }

    delete engine;
}

void JsPropertyTest::jsDynamicPropertyWithAddedExpression(){
    Engine* engine = new Engine();
    {
        engine->scope([engine](){
            Element* e = new Element(engine);
            e->ref();

            Element* x = new Element(engine);
            x->addProperty("value", "int", LocalValue(engine, 200), false, true, "valueChanged");
            QVERIFY(x->hasProperty("value"));

            Element* y = new Element(engine);
            y->addProperty("value", "int", LocalValue(engine, 200), false, true, "valueChanged");
            QVERIFY(y->hasProperty("value"));

            LocalObject globalObject(engine->currentContext());
            globalObject.set(engine, "e", LocalValue(engine, e));
            globalObject.set(engine, "x", LocalValue(engine, x));
            globalObject.set(engine, "y", LocalValue(engine, y));

            Script::Ptr s = engine->compile(
                "Element.addProperty(e, 'p', {"
                    "type: 'int',"
                    "value: function(){ return x.value + y.value; },"
                    "isWritable: true,"
                    "isDefault: false,"
                    "bindings : [[x, 'valueChanged'], [y,'valueChanged']],"
                    "notify: 'pChanged'"
                "});"
            );
            s->run();

            QCOMPARE(e->get("p").toInt32(engine), 400);

            x->set("value", LocalValue(engine, 500));
            QCOMPARE(e->get("p").toInt32(engine), 700);

            y->set("value", LocalValue(engine, 400));
            QCOMPARE(e->get("p").toInt32(engine), 900);

            // Reset the binding
            e->set("p", LocalValue(engine, 200));
            QCOMPARE(e->get("p").toInt32(engine), 200);

            // Check binding reset
            x->set("value", LocalValue(engine, 300));
            QCOMPARE(e->get("p").toInt32(engine), 200);

            delete e;
        });
    }

    delete engine;
}
