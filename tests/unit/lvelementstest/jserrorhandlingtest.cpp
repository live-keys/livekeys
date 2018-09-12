#include "jserrorhandlingtest.h"
#include "live/elements/element.h"
#include "live/elements/engine.h"

Q_TEST_RUNNER_REGISTER(JsErrorHandlingTest);

using namespace lv;
using namespace lv::el;

JsErrorHandlingTest::JsErrorHandlingTest(QObject* parent)
    : QObject(parent)
{

}

void JsErrorHandlingTest::initTestCase(){
}
