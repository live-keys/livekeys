#include "jstypestest.h"
#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/modulelibrary.h"
#include "live/elements/component.h"

Q_TEST_RUNNER_REGISTER(JsTypesTest);

using namespace lv;
using namespace lv::el;

JsTypesTest::JsTypesTest(QObject *parent)
    : QObject(parent)
{
}

void JsTypesTest::initTestCase(){
}

void JsTypesTest::pointTypeTest(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){
            Value v = engine->compileEnclosed("return new Point(20, 20);")->run();
            Object o = v.asObject();
            QVERIFY(o.isPoint());
            double x, y;
            o.toPoint(x, y);
            QVERIFY(x == 20 && y == 20);

            o = engine->compileEnclosed("class TT extends Point{}; return new TT();")->run().asObject();
            QVERIFY(o.isPoint());
        });
    }
    delete engine;
}

void JsTypesTest::sizeTypeTest(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){
            Value v = engine->compileEnclosed("return new Size(20, 20);")->run();
            Object o = v.asObject();
            QVERIFY(o.isSize());
            double w, h;
            o.toSize(w, h);
            QVERIFY(w == 20 && h == 20);

            o = engine->compileEnclosed("class TT extends Size{}; return new TT();")->run().asObject();
            QVERIFY(o.isSize());
        });
    }
    delete engine;
}

void JsTypesTest::rectangleTypeTest(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){
            Value v = engine->compileEnclosed("return new Rectangle(1, 2, 3, 4);")->run();
            Object o = v.asObject();
            QVERIFY(o.isRectangle());
            double x, y, w, h;
            o.toRectangle(x, y, w, h);
            QVERIFY(x == 1 && y == 2);
            QVERIFY(w == 3 && h == 4);

            o = engine->compileEnclosed("class TT extends Rectangle{}; return new TT();")->run().asObject();
            QVERIFY(o.isRectangle());
        });
    }
    delete engine;
}

void JsTypesTest::dateTypeTest(){
    Engine* engine = new Engine();
    {
        engine->scope([&engine](){
            Value v = engine->compileEnclosed("return new Date();")->run();
            Object o = v.asObject();
            QVERIFY(o.isDate());
        });
    }
    delete engine;
}
