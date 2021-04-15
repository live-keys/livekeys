#include "jstupletest.h"
#include "live/elements/engine.h"
#include "live/elements/script.h"

Q_TEST_RUNNER_REGISTER(JsTupleTest);

using namespace lv;
using namespace lv::el;

JsTupleTest::JsTupleTest(QObject *parent)
    : QObject(parent)
{
}

JsTupleTest::~JsTupleTest(){
}

void JsTupleTest::initTestCase(){

}

void JsTupleTest::propertyAdditionTest(){
    Engine* engine = new Engine();
    {
        engine->scope([engine](){
            Script::Ptr s = engine->compileJsEnclosed(
                "var t = new Tuple();"
                "var propertiesChanged = [];"
                "t.on('propertyChanged', function(name){ propertiesChanged.push(name); });"
                "Element.addProperty(t, 'p', {"
                    "type: 'int',"
                    "value: 20,"
                    "isWritable: true,"
                    "isDefault: false,"
                    "notify: 'pChanged'"
                "});"
                "Element.addProperty(t, 'x', {"
                    "type: 'int',"
                    "value: 200,"
                    "isWritable: true,"
                    "isDefault: false,"
                    "notify: 'xChanged'"
                "});"
                "t.p = 200;"
                "t.x = 100;"
                "return propertiesChanged;"
            );
            Value v = s->run();
            Object o = v.asObject();
            Object::Accessor lo(o);

            int totalPropertiesChanged = lo.get(engine, "length").toInt32(engine);
            QVERIFY(totalPropertiesChanged == 2);

            QVERIFY(lo.get(engine, 0).toStdString(engine) == "p");
            QVERIFY(lo.get(engine, 1).toStdString(engine) == "x");
        });
    }

    delete engine;
}
