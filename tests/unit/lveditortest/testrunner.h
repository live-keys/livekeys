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

#ifndef LVTESTRUNNER_H
#define LVTESTRUNNER_H

#include "qglobal.h"
#include <QObject>
#include <QList>
#include <QSharedPointer>
#include <QTest>
#include <iostream>

namespace lv{

class TestRunner{

public:
    static int registerTest(QObject* test);
    static int runTests(int argc, char *argv[]);
    static int runTest(int index, int argc, char* argv[]);
    static int totalRegisteredTests();

private:
    static QList<QSharedPointer<QObject> >& tests();
};

inline int TestRunner::registerTest(QObject* test){
    tests().append(QSharedPointer<QObject>(test));
    return tests().size() - 1;
}

inline int TestRunner::runTests(int argc, char *argv[]){
    int code = 0;
    std::cout << "3";
    for ( QList<QSharedPointer<QObject> >::iterator it = tests().begin(); it != tests().end(); ++it ){
        code += QTest::qExec(it->data(), argc, argv);
    }
    return code;
}

inline int TestRunner::runTest(int index, int argc, char* argv[]){
    if ( index > tests().size() )
        return -1;
    return QTest::qExec(tests()[index].data(), argc, argv);
}


inline int TestRunner::totalRegisteredTests(){
    return tests().size();
}

inline QList<QSharedPointer<QObject> > &TestRunner::tests(){
    static QList<QSharedPointer<QObject> > registeredTests;
    return registeredTests;
}

}// namespace

#define Q_TEST_RUNNER_SUITE \
    public:\
        static const int testIndex;

#define Q_TEST_RUNNER_REGISTER(className) \
    const int className::testIndex = lv::TestRunner::registerTest(new className)

#endif // LVTESTRUNNER_H
