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

#ifndef JSOBJECTTEST_H
#define JSOBJECTTEST_H

#include <QObject>
#include "testrunner.h"

class JsObjectTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    JsObjectTest(QObject* parent = 0);
    ~JsObjectTest(){}

private slots:
    void initTestCase();
    void simpleObjectInitializationTest();

};

#endif // JSOBJECTTEST_H
