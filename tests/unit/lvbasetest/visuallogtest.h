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

#ifndef VISUALLOGTEST_H
#define VISUALLOGTEST_H

#include <QObject>
#include "testrunner.h"

namespace lv{
class VisualLogModel;
}
class QQmlEngine;
class QCoreApplication;

class VisualLogTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit VisualLogTest(QObject *parent = 0);
    ~VisualLogTest();

private slots:
    void initTestCase();
    void cleanupTestCase();

    void levelTest();
    void levelObjectTest();
    void prefixTest();
    void fileOutputTest();
    void dailyFileOutputTest();
    void viewOutputTest();

private:
    QQmlEngine*          m_engine;
    lv::VisualLogModel* m_vlogModel;

};

#endif // VISUALLOGTEST_H
