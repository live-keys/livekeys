#include "jslisttest.h"
#include "live/elements/event.h"
#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/list.h"

Q_TEST_RUNNER_REGISTER(JsListTest);

using namespace lv;
using namespace lv::el;


class WritableListStub{


public:
    static List* createList(Engine* engine, std::vector<int>* data){
        return new List(engine, data, &at, &length, &assign, &append, &clear);
    }

    static LocalValue at(List* l, int index){
        return LocalValue(l->engine(), l->dataAs<std::vector<int>*>()->at(index));
    }
    static int length(List* l){
        return static_cast<int>(l->dataAs<std::vector<int>*>()->size());
    }
    static void assign(List* l, int index, LocalValue value){
        (*l->dataAs<std::vector<int>*>())[index] = value.toInt32(l->engine());

    }
    static void append(List* l, LocalValue value){
        l->dataAs<std::vector<int>*>()->push_back(value.toInt32(l->engine()));
    }
    static void clear(List* l){
        l->dataAs<std::vector<int>*>()->clear();
    }
};

JsListTest::JsListTest(QObject *parent)
    : QObject(parent)
{
}

void JsListTest::initTestCase(){
}

void JsListTest::testFunctions(){


    Engine* engine = new Engine();
    {
        engine->scope([engine](){
            std::vector<int> listData;

            List* l = WritableListStub::createList(engine, &listData);
            l->ref();

            LocalObject globalObject(engine->currentContext());
            globalObject.set(engine, "l", LocalValue(engine, l));

            engine->compileJsEnclosed("l.append(20);")->run();
            QVERIFY(l->length() == 1);
            QVERIFY(l->at(0).toInt32(engine) == 20);
            engine->compileJsEnclosed("l.append(2);")->run();
            QVERIFY(l->length() == 2);
            QVERIFY(l->at(1).toInt32(engine) == 2);
            engine->compileJsEnclosed("l.assign(0, 11);")->run();
            QVERIFY(l->at(0).toInt32(engine) == 11);
            QVERIFY(listData[0] == 11);
            QVERIFY(engine->compileJsEnclosed("return l.at(0);")->run().asInt32() == 11);
            QVERIFY(engine->compileJsEnclosed("return l[0];")->run().asInt32() == 11);

            engine->compileJsEnclosed("l[0] = 15;")->run();
            QVERIFY(engine->compileJsEnclosed("return l[0];")->run().asInt32() == 15);

            QVERIFY(engine->compileJsEnclosed("return l.length;")->run().asInt32() == 2);

            delete l;
        });
    }

    delete engine;

}

void JsListTest::testEvents(){

}
