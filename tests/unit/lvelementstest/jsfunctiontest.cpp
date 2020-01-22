#include "jsfunctiontest.h"

#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/component.h"
#include "live/elements/modulelibrary.h"

Q_TEST_RUNNER_REGISTER(JsFunctionTest);

using namespace lv;
using namespace lv::el;


class FunctionStub : public Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(FunctionStub)
            .base<Element>()
            .constructor<>()
            .scriptFunction("boolReturnType", &FunctionStub::boolReturnType)
            .scriptFunction("intReturnType",   &FunctionStub::intReturnType)
            .scriptFunction("int64ReturnType", &FunctionStub::int64ReturnType)
            .scriptFunction("numberReturnType", &FunctionStub::numberReturnType)
            .scriptFunction("stringReturnType", &FunctionStub::stringReturnType)
            .scriptFunction("callableReturnType", &FunctionStub::callableReturnType)
            .scriptFunction("objectReturnType", &FunctionStub::objectReturnType)
            .scriptFunction("localValueReturnType", &FunctionStub::localValueReturnType)
            .scriptFunction("valueReturnType", &FunctionStub::valueReturnType)
            .scriptFunction("bufferReturnType", &FunctionStub::bufferReturnType)
            .scriptFunction("elementReturnType", &FunctionStub::elementReturnType)
            .scriptFunction("userElementReturnType", &FunctionStub::userElementReturnType)
            .scriptFunction("intNoParameterReturnType", &FunctionStub::intNoParameterReturnType)
            .scriptFunction("voidReturnType", &FunctionStub::voidReturnType)
            .scriptFunction("variableParametersType", &FunctionStub::variableParametersType)
            .scriptFunction("variableParametersVoidType", &FunctionStub::variableParametersVoidType)
        META_OBJECT_CLOSE
    }

    static bool boolReturnType(bool value){ return value; }
    static Value::Int32 intReturnType(Value::Int32 value){ return value; }
    static Value::Int64 int64ReturnType(Value::Int64 value){ return value; }
    static Value::Number numberReturnType(Value::Number value){ return value; }
    static const std::string& stringReturnType(const std::string& value){ return value;}
    static Callable callableReturnType(const Callable& value){ return value; }
    static Object objectReturnType(const Object& value){ return value; }
    static LocalValue localValueReturnType(LocalValue value){ return value; }
    static Value valueReturnType(Value value){ return value; }
    static Buffer bufferReturnType(Buffer value){ return value; }
    static Element* elementReturnType(Element* value){ return value;}
    static FunctionStub* userElementReturnType(FunctionStub* value){ return value; }

    static Value::Int32 intNoParameterReturnType(){ return 200; }

    static void voidReturnType(Value::Int32 v1, Value::Number v2){ forwardCall()(v1, v2, "");}

    static int variableParametersType(const Function::CallInfo& p){
        forwardCall()(p.at(0).toInt32(p.engine()), p.at(1).toNumber(p.engine()), p.at(2).toStdString(p.engine()));
        return (int)p.length();
    }

    static void variableParametersVoidType(const Function::CallInfo& p){
        forwardCall()(p.at(0).toInt32(p.engine()), p.at(1).toNumber(p.engine()), p.at(2).toStdString(p.engine()));
    }

public:
    FunctionStub(Engine* engine) : Element(engine){}

    static std::function<void(Value::Int32, Value::Number, const std::string&)>& forwardCall(){
        static std::function<void(Value::Int32, Value::Number, const std::string&)> f;
        return f;
    }
};

JsFunctionTest::JsFunctionTest(QObject *parent) : QObject(parent)
{
}

JsFunctionTest::~JsFunctionTest(){
}

void JsFunctionTest::initTestCase(){
}

void JsFunctionTest::typesTest(){

    Engine* engine = new Engine();

    engine->scope([engine](){
        ElementsPlugin::Ptr epl = ElementsPlugin::create(Plugin::createEmpty("test"), engine);

        ModuleLibrary* m = ModuleLibrary::create(engine, "");
        m->addType<FunctionStub>();
        epl->addModuleLibrary(m);

        Object o  = epl->collectExportsObject();
        LocalObject lo(o);

        FunctionStub* fs = new FunctionStub(engine);
        fs->ref();

        LocalObject globalObject(engine->currentContext());
        globalObject.set(engine, "e", LocalValue(engine, fs));
        globalObject.set(engine, "FunctionStub", lo.get(engine, "FunctionStub"));

        Value v = engine->compileJsEnclosed("return FunctionStub.boolReturnType(true);")->run();
        QCOMPARE(v.asBool(), true);

        v = engine->compileJsEnclosed("return FunctionStub.intReturnType(20);")->run();
        QCOMPARE(v.asInt32(), 20);

        v = engine->compileJsEnclosed("return FunctionStub.int64ReturnType(40);")->run();
        QCOMPARE(v.asInt64(), 40);

        v = engine->compileJsEnclosed("return FunctionStub.numberReturnType(20.2);")->run();
        QCOMPARE(v.asNumber(), 20.2);

        v = engine->compileJsEnclosed("return FunctionStub.stringReturnType('123');")->run();
        QVERIFY(v.asObject().toString() == "123");

        v = engine->compileJsEnclosed("return FunctionStub.callableReturnType(function(){ return 2; });")->run();
        QVERIFY(v.type() == Value::Stored::Callable);
        QVERIFY(v.asCallable().call(engine, Function::Parameters(0)).toInt32(engine) == 2);

        v = engine->compileJsEnclosed("return FunctionStub.objectReturnType({a: 2, b: 30});")->run();
        QVERIFY(v.type() == Value::Stored::Object);
        LocalObject vo(v.asObject());
        QVERIFY(vo.get(engine, "a").toInt32(engine) == 2);
        QVERIFY(vo.get(engine, "b").toInt32(engine) == 30);

        v = engine->compileJsEnclosed("return FunctionStub.localValueReturnType(2);")->run();
        QVERIFY(v.asInt32() == 2);

        v = engine->compileJsEnclosed("return FunctionStub.valueReturnType('123');")->run();
        QVERIFY(v.asObject().toString() == "123");

        v = engine->compileJsEnclosed("return FunctionStub.bufferReturnType(new ArrayBuffer(8));")->run();
        QVERIFY(v.asObject().toBuffer().size() == 8);

        v = engine->compileJsEnclosed("return FunctionStub.elementReturnType(e);")->run();
        QVERIFY(v.asElement() == fs);

        v = engine->compileJsEnclosed("return FunctionStub.userElementReturnType(e);")->run();
        QVERIFY(v.asElement() == fs);

        v = engine->compileJsEnclosed("return FunctionStub.intNoParameterReturnType();")->run();
        QVERIFY(v.asInt32() == 200);

        Value::Int32 intReturn     = 0;
        Value::Number numberReturn = 0;
        FunctionStub::forwardCall() = [&intReturn, &numberReturn](Value::Int32 v1, Value::Number v2, const std::string&){
            intReturn = v1;
            numberReturn = v2;
        };
        engine->compileJsEnclosed("return FunctionStub.voidReturnType(2, 20.2);")->run();
        QCOMPARE(intReturn, 2);
        QCOMPARE(numberReturn, 20.2);

        Value::Int32 param0  = 0;
        Value::Number param1 = 0;
        std::string param2;
        FunctionStub::forwardCall() = [&param0, &param1, &param2](Value::Int32 v1, Value::Number v2, const std::string& v3){
            param0 = v1;
            param1 = v2;
            param2 = v3;
        };
        v = engine->compileJsEnclosed("return FunctionStub.variableParametersType(3, 30.3, '123');")->run();
        QCOMPARE(v.asInt32(), 3);
        QVERIFY(param0 == 3);
        QVERIFY(param1 == 30.3);
        QVERIFY(param2 == "123");

        v = engine->compileJsEnclosed("FunctionStub.variableParametersVoidType(4, 40.4, '1234');")->run();
        QVERIFY(v.isNull());
        QVERIFY(param0 == 4);
        QVERIFY(param1 == 40.4);
        QVERIFY(param2 == "1234");


        delete fs;
    });

    delete engine;
}
