/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#include "jsobjecttest.h"
#include "live/elements/element.h"
#include "live/elements/metaobject.h"
#include "live/elements/component.h"
#include "live/elements/modulelibrary.h"
#include <memory.h>

Q_TEST_RUNNER_REGISTER(JsObjectTest);

using namespace lv;
using namespace lv::el;


class ElementStub : public el::Element{

    META_OBJECT{
        META_OBJECT_DESCRIBE(ElementStub)
            .base<Element>()
            .constructor<int>()
            .scriptMethod("getData", &ElementStub::getData)
            .scriptProperty<int>("t", &ElementStub::t, &ElementStub::setT, "tChanged")
            .scriptEvent("tChanged", &ElementStub::tChanged)
        META_OBJECT_CLOSE
    }

public:
    ElementStub(el::Engine* engine, int t = 50)
        : Element(engine), m_t(t)
    {
    }

    int getData() const{ return 20; }

    int t(){ return m_t; }
    void setT(int t){ m_t = t; }

public:
    Event initialized(){ return notify(eventId(&ElementStub::initialized)); }
    Event tChanged(){ return notify(eventId(&ElementStub::tChanged)); }

private:
    int m_t;
};

JsObjectTest::JsObjectTest(QObject *parent)
    : QObject(parent)
{
}

void JsObjectTest::initTestCase(){
}

void JsObjectTest::simpleObjectInitializationTest(){
    Engine* engine = new Engine();

    ModuleLibrary* m = ModuleLibrary::create(engine, "test");
    m->addType<ElementStub>();

    {
        engine->scope([&engine, m](){
            el::Object ob = engine->require(m);

            LocalObject lo(ob);
            LocalValue v = lo.get(engine, "ElementStub");
            QVERIFY(v.isCallable());

            Callable c = v.toCallable(engine);
            QVERIFY(c.isComponent());

            Component comp = c.toComponent();

            Function::Parameters p(1);
            p.assign(0, LocalValue(engine, 15));

            Element* el = comp.create(p);
            QCOMPARE(std::string("ElementStub"), el->typeMetaObject().name());

            ElementStub* elst = static_cast<ElementStub*>(el);
            QCOMPARE(elst->t(), 15);
        });
    }


    delete engine;
}


