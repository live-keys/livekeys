/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
** This file is part of Livekeys Application.
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

#include "viewengineteststub.h"
#include "live/exception.h"
#include <QQmlContext>
#include <QQmlEngine>

ViewEngineTestStub::ViewEngineTestStub(QObject *parent)
    : QObject(parent)
{
}

void ViewEngineTestStub::throwException(){
    THROW_EXCEPTION(lv::Exception, "Exception stub.", 0);
}

void ViewEngineTestStub::throwJsError(){
    QObject* engineObj = qmlContext(this)->contextProperty("engine").value<QObject*>();
    lv::ViewEngine* engine = qobject_cast<lv::ViewEngine*>(engineObj);
    if ( !engine )
        return;

    lv::Exception exception = lv::Exception::create<lv::Exception>(
        "JSTest", 1, "enginetest.cpp", 100, "jsExceptionInObjectTest"
    );
    engine->throwError(&exception, this);
}
