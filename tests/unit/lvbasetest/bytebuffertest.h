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

#ifndef BYTEBUFFERTEST_H
#define BYTEBUFFERTEST_H

#include <QObject>
#include "testrunner.h"

class ByteBufferTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    ByteBufferTest(QObject* parent = 0);
    ~ByteBufferTest();

private slots:
    void initTestCase();
    void testEncode();
    void testRoundTrip();
};

#endif // BYTEBUFFERTEST_H
