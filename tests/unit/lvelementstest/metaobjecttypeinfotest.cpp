#include "metaobjecttypeinfotest.h"
#include "testpack.h"

Q_TEST_RUNNER_REGISTER(MetaObjectTypeInfoTest);


#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/container.h"
#include "live/elements/languageinfo.h"
#include "live/visuallog.h"
#include "live/typename.h"

using namespace lv;
using namespace lv::el;

MetaObjectTypeInfoTest::MetaObjectTypeInfoTest(QObject *parent)
    : QObject(parent)
{
}

void MetaObjectTypeInfoTest::initTestCase(){
}

void MetaObjectTypeInfoTest::elementTest(){
    TypeInfo::Ptr ti = TypeInfo::extract(Element::metaObject(), "base");
    QVERIFY(ti->className() == "cpp/lv::el::Element");
    QVERIFY(ti->typeName() == "lv/base#Element");
    QVERIFY(ti->inheritsName() == "");
    QVERIFY(ti->getConstructor().name() == "constructor");
    QVERIFY(ti->getConstructor().returnType() == "");
    QVERIFY(ti->getConstructor().parameterCount() == 0);
    QVERIFY(ti->totalProperties() == 1);
    QVERIFY(ti->propertyAt(0).name() == "parent");
    QVERIFY(ti->propertyAt(0).typeName() == "cpp/lv::el::Element*");
}

void MetaObjectTypeInfoTest::containerTest(){
    TypeInfo::Ptr ti = TypeInfo::extract(Container::metaObject(), "base");
    QVERIFY(ti->className() == "cpp/lv::el::Container");
    QVERIFY(ti->typeName() == "lv/base#Container");
    QVERIFY(ti->inheritsName() == "cpp/lv::el::Element");
    QVERIFY(ti->getConstructor().name() == "constructor");
    QVERIFY(ti->getConstructor().returnType() == "");
    QVERIFY(ti->getConstructor().parameterCount() == 0);
    QVERIFY(ti->totalProperties() == 1);
    QVERIFY(ti->propertyAt(0).name() == "children");
    QVERIFY(ti->propertyAt(0).typeName() == "cpp/lv::el::LocalValue");
}
