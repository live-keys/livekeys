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

#ifndef ENGINETEST_H
#define ENGINETEST_H

#include <QObject>
#include "testrunner.h"

class EngineTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    EngineTest(QObject* parent = 0);
    ~EngineTest(){}

private slots:
    void initTestCase();
    void cppExceptionInObbjectTest();
    void engineExceptionTest();
    void engineWarningTest();

    void engineObjectExceptionTest();
    void engineInternalWarningTest();

    void engineErrorHandlerTest();
    void engineErrorHandlerSkipTest();

    void jsThrownErrorTest();
    void jsThrownErrorHandlerTest();

};

#endif // ENGINETEST_H
