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

#include "engineteststub.h"
#include "live/exception.h"
#include <QQmlContext>
#include <QQmlEngine>

EngineTestStub::EngineTestStub(QObject *parent)
    : QObject(parent)
{
}

void EngineTestStub::throwException(){
    THROW_EXCEPTION(lv::Exception, "Exception stub.", 0);
}

void EngineTestStub::throwJsError(){
    QObject* engineObj = qmlContext(this)->contextProperty("engine").value<QObject*>();
    lv::ViewEngine* engine = qobject_cast<lv::ViewEngine*>(engineObj);
    if ( !engine )
        return;

    lv::Exception exception = lv::Exception::create<lv::Exception>(
        "JSTest", 1, "enginetest.cpp", 100, "jsExceptionInObjectTest"
    );
    engine->throwError(&exception, this);
}

void EngineTestStub::throwJsWarning(){
    QObject* engineObj = qmlContext(this)->contextProperty("engine").value<QObject*>();
    lv::ViewEngine* engine = qobject_cast<lv::ViewEngine*>(engineObj);
    if ( !engine )
        return;

    engine->throwWarning("JSTest", this, "enginetest.cpp", 100);
}
