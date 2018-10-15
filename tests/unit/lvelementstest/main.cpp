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

#include <QCoreApplication>
#include <QTest>
#include <qqml.h>

#include "live/elements/engine.h"

#include "testrunner.h"
#include "eventtest.h"
#include "jsobjecttest.h"
#include "jspropertytest.h"
#include "jseventtest.h"
#include "jsfunctiontest.h"
#include "jsmethodtest.h"
#include "jstypestest.h"
#include "jsmemorytest.h"
#include "jslisttest.h"
#include "jserrorhandlingtest.h"
#include "lvcompiletest.h"


int main(int argc, char *argv[]){

    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);

    lv::el::Engine::InitializeScope initializer(QDir::currentPath().toStdString());

    return lv::TestRunner::runTests(argc, argv);
}
