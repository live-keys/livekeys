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

#ifndef MLNODETOJSTEST_H
#define MLNODETOJSTEST_H

#include <QObject>
#include "testrunner.h"
#include "live/mlnode.h"
#include "live/mlnodetojs.h"

class MLNodeToJsTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit MLNodeToJsTest(QObject *parent = 0);
    ~MLNodeToJsTest();

private slots:
    void initTestCase();
    void jsConvertTest();

};

#endif // MLNODETOJSTEST_H
