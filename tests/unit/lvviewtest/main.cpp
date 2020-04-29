/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include <QCoreApplication>
#include <QTest>
#include <qqml.h>

#include "testrunner.h"
#include "mlnodetoqmltest.h"
#include "visuallogtest.h"
#include "grouptest.h"
#include "linecapturetest.h"
#include "memorytest.h"

int main(int argc, char *argv[]){

    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);


    return lv::TestRunner::runTest(MemoryTest::testIndex, argc, argv);
    return lv::TestRunner::runTests(argc, argv);
}
