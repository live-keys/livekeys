#include "metaobjecttypeinfotest.h"
#include "testpack.h"

Q_TEST_RUNNER_REGISTER(MetaObjectTypeInfoTest);


#include "live/elements/engine.h"
#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/container.h"
#include "live/elements/metaobjecttypeinfo.h"
#include "live/elements/compiler/languageinfo.h"
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
    TypeInfo::Ptr ti = MetaObjectTypeInfo::extract(Element::metaObject(), "base");
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
    TypeInfo::Ptr ti = MetaObjectTypeInfo::extract(Container::metaObject(), "base");
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

void MetaObjectTypeInfoTest::typeReferenceTest(){
    QVERIFY(TypeReference::languageString(TypeReference::languageId("qml")) == "qml");
    QVERIFY(TypeReference::languageString(TypeReference::languageId("cpp")) == "cpp");

    TypeReference trtest;
    QVERIFY(trtest.languageString() == "u");

    TypeReference trtest2 = TypeReference::split("qml/lvbase#Act");
    QVERIFY(trtest2.languageString() == "qml");
    QVERIFY(trtest2.path() == "lvbase");
    QVERIFY(trtest2.name() == "Act");
    QVERIFY(trtest2.join() == "qml/lvbase#Act");

    TypeReference trtest3 = TypeReference::split("Act");
    QVERIFY(trtest3.languageString() == "u");
    QVERIFY(trtest3.path() == "");
    QVERIFY(trtest3.name() == "Act");
    QVERIFY(trtest3.join() == "u/#Act");

    TypeReference trtest4 = TypeReference::split("u/#Act");
    QVERIFY(trtest4.languageString() == "u");
    QVERIFY(trtest4.path() == "");
    QVERIFY(trtest4.name() == "Act");
    QVERIFY(trtest4.join() == "u/#Act");
}
