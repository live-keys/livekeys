#include "metaobjectfunctiontest.h"
#include "live/elements/metaobject.h"

Q_TEST_RUNNER_REGISTER(MetaObjectFunctionTest);

using namespace lv;
using namespace lv::el;

class FunctionArgumentsStub{

public:
    int incrementNumber(int n){ return ++n; }

};

MetaObjectFunctionTest::MetaObjectFunctionTest(QObject *parent)
    : QObject(parent)
{
}

void MetaObjectFunctionTest::initTestCase(){

}

void MetaObjectFunctionTest::simpleFunctionTest(){
    MetaObject::Builder<FunctionArgumentsStub> mob("SimplePointStub");
    mob.scriptMethod("incrementNumber", &FunctionArgumentsStub::incrementNumber);

    MetaObject mo = MetaObject::fromBuilder(mob);

    QCOMPARE((int)(mo.totalMethods()), 1);

    Function* f = mo.getMethod("incrementNumber");
    QVERIFY(f != nullptr);
    QVERIFY(f->totalArguments() == 1);
    QVERIFY(f->less() == nullptr);
    Method<FunctionArgumentsStub, int, int>* sf = static_cast<Method<FunctionArgumentsStub, int, int>*>(f);
    QVERIFY(sf != 0);

    FunctionArgumentsStub sfs;
    QVERIFY(sf->callOn(&sfs, 20) == 21);
}

void MetaObjectFunctionTest::optionalArgumentsTypesTest()
{

}
