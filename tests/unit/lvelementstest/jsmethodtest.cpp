#include "jsmethodtest.h"

#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/component.h"
#include "live/elements/module.h"

Q_TEST_RUNNER_REGISTER(JsMethodTest);

using namespace lv;
using namespace lv::el;

class MethodStub : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(MethodStub)
            .base<Element>()
            .constructor<>()
            .scriptMethod("boolReturnType", &MethodStub::boolReturnType)
            .scriptMethod("intReturnType",   &MethodStub::intReturnType)
            .scriptMethod("int64ReturnType", &MethodStub::int64ReturnType)
            .scriptMethod("numberReturnType", &MethodStub::numberReturnType)
            .scriptMethod("stringReturnType", &MethodStub::stringReturnType)
            .scriptMethod("callableReturnType", &MethodStub::callableReturnType)
            .scriptMethod("objectReturnType", &MethodStub::objectReturnType)
            .scriptMethod("localValueReturnType", &MethodStub::localValueReturnType)
            .scriptMethod("valueReturnType", &MethodStub::valueReturnType)
            .scriptMethod("bufferReturnType", &MethodStub::bufferReturnType)
            .scriptMethod("elementReturnType", &MethodStub::elementReturnType)
            .scriptMethod("userElementReturnType", &MethodStub::userElementReturnType)
            .scriptMethod("intNoParameterReturnType", &MethodStub::intNoParameterReturnType)
            .scriptMethod("voidReturnType", &MethodStub::voidReturnType)
            .scriptMethod("variableParametersType", &MethodStub::variableParametersType)
            .scriptMethod("variableParametersVoidType", &MethodStub::variableParametersVoidType)
        META_OBJECT_CLOSE
    }

    bool boolReturnType(bool value){ return value; }
    Value::Int32 intReturnType(Value::Int32 value){ return value; }
    Value::Int64 int64ReturnType(Value::Int64 value){ return value; }
    Value::Number numberReturnType(Value::Number value){ return value; }
    const std::string& stringReturnType(const std::string& value){ return value;}
    Callable callableReturnType(const Callable& value){ return value; }
    Object objectReturnType(const Object& value){ return value; }
    LocalValue localValueReturnType(LocalValue value){ return value; }
    Value valueReturnType(Value value){ return value; }
    Buffer bufferReturnType(Buffer value){ return value; }
    Element* elementReturnType(Element* value){ return value;}
    MethodStub* userElementReturnType(MethodStub* value){ return value; }

    Value::Int32 intNoParameterReturnType(){ return 200; }

    void voidReturnType(Value::Int32 v1, Value::Number v2){ forwardCall()(v1, v2, "");}

    int variableParametersType(const Function::CallInfo& p){
        forwardCall()(p.at(0).toInt32(p.engine()), p.at(1).toNumber(p.engine()), p.at(2).toStdString(p.engine()));
        return p.length();
    }

    void variableParametersVoidType(const Function::CallInfo& p){
        forwardCall()(p.at(0).toInt32(p.engine()), p.at(1).toNumber(p.engine()), p.at(2).toStdString(p.engine()));
    }

    // optional params

public:
    MethodStub(Engine* engine) : Element(engine){}

    static std::function<void(Value::Int32, Value::Number, const std::string&)>& forwardCall(){
        static std::function<void(Value::Int32, Value::Number, const std::string&)> f;
        return f;
    }
};


JsMethodTest::JsMethodTest(QObject *parent)
    : QObject(parent)
{
}

JsMethodTest::~JsMethodTest(){
}

void JsMethodTest::initTestCase(){

}

void JsMethodTest::typesTest(){

    Engine* engine = new Engine();
    Module::Ptr m = Module::create("test", 1, 0);
    m->addType<MethodStub>();

    {
        engine->scope([engine, m](){
            Object o  = engine->require(m);
            LocalObject lo(o);

            MethodStub* e = new MethodStub(engine);
            e->ref();

            LocalObject globalObject(engine->currentContext());
            globalObject.set(engine, "e", LocalValue(engine, e));

            Value v = engine->compileEnclosed("return e.intReturnType(20);")->run();
            QCOMPARE(v.asInt32(), 20);

            v = engine->compileEnclosed("return e.int64ReturnType(40);")->run();
            QCOMPARE(v.asInt64(), 40);

            v = engine->compileEnclosed("return e.numberReturnType(20.2);")->run();
            QCOMPARE(v.asNumber(), 20.2);

            v = engine->compileEnclosed("return e.stringReturnType('123');")->run();
            QVERIFY(v.asObject().toString() == "123");

            v = engine->compileEnclosed("return e.callableReturnType(function(){ return 2; });")->run();
            QVERIFY(v.type() == Value::Stored::Callable);
            QVERIFY(v.asCallable().call(engine, Function::Parameters(0)).toInt32(engine) == 2);

            v = engine->compileEnclosed("return e.objectReturnType({a: 2, b: 30});")->run();
            QVERIFY(v.type() == Value::Stored::Object);
            LocalObject vo(v.asObject());
            QVERIFY(vo.get(engine, "a").toInt32(engine) == 2);
            QVERIFY(vo.get(engine, "b").toInt32(engine) == 30);

            v = engine->compileEnclosed("return e.localValueReturnType(2);")->run();
            QVERIFY(v.asInt32() == 2);

            v = engine->compileEnclosed("return e.valueReturnType('123');")->run();
            QVERIFY(v.asObject().toString() == "123");

            v = engine->compileEnclosed("return e.bufferReturnType(new ArrayBuffer(8));")->run();
            QVERIFY(v.asObject().toBuffer().size() == 8);

            v = engine->compileEnclosed("return e.elementReturnType(e);")->run();
            QVERIFY(v.asElement() == e);

            v = engine->compileEnclosed("return e.userElementReturnType(e);")->run();
            QVERIFY(v.asElement() == e);

            v = engine->compileEnclosed("return e.intNoParameterReturnType();")->run();
            QVERIFY(v.asInt32() == 200);

            Value::Int32 intReturn     = 0;
            Value::Number numberReturn = 0;
            MethodStub::forwardCall() = [&intReturn, &numberReturn](Value::Int32 v1, Value::Number v2, const std::string&){
                intReturn = v1;
                numberReturn = v2;
            };
            engine->compileEnclosed("e.voidReturnType(2, 20.2);")->run();
            QCOMPARE(intReturn, 2);
            QCOMPARE(numberReturn, 20.2);

            Value::Int32 param0  = 0;
            Value::Number param1 = 0;
            std::string param2;
            MethodStub::forwardCall() = [&param0, &param1, &param2](Value::Int32 v1, Value::Number v2, const std::string& v3){
                param0 = v1;
                param1 = v2;
                param2 = v3;
            };
            v = engine->compileEnclosed("return e.variableParametersType(3, 30.3, '123');")->run();
            QCOMPARE(v.asInt32(), 3);
            QVERIFY(param0 == 3);
            QVERIFY(param1 == 30.3);
            QVERIFY(param2 == "123");

            v = engine->compileEnclosed("e.variableParametersVoidType(4, 40.4, '1234');")->run();
            QVERIFY(v.isNull());
            QVERIFY(param0 == 4);
            QVERIFY(param1 == 40.4);
            QVERIFY(param2 == "1234");


            delete e;
        });
    }

}
