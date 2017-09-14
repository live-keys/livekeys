/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef QTESTRUNNER_H
#define QTESTRUNNER_H

#include <QObject>
#include <QList>
#include <QSharedPointer>

class QTestRunner{

public:
    static int registerTest(QObject* test);
    static int runTests(int argc, char *argv[]);
    static int runTest(int index, int argc, char* argv[]);
    static int totalRegisteredTests();
    static void cleanupTests();

private:
    static QList<QSharedPointer<QObject> >* m_tests;
};

inline int QTestRunner::totalRegisteredTests(){
    return m_tests->size();
}

#define Q_TEST_RUNNER_SUITE \
    public:\
        static const int testIndex;

#define Q_TEST_RUNNER_REGISTER(className) \
    const int className::testIndex = QTestRunner::registerTest(new className)

#endif // QTESTRUNNER_H
