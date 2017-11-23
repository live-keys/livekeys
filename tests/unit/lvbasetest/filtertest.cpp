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

#include "filtertest.h"
#include "filterteststub.h"
#include "live/filter.h"
#include "live/filterworker.h"

using namespace lv;

Q_TEST_RUNNER_REGISTER(FilterTest);

FilterTest::FilterTest(QObject *parent)
    : QObject(parent)
{
}

void FilterTest::initTestCase()
{
}

void FilterTest::testOneProducerOneFilter(){
//    SharedDataTestStub* i1 = new SharedDataTestStub;
//    SharedDataTestStub* i2 = new SharedDataTestStub;
//    for ( int i = 1; i <= 5; ++i ){
//        i1->items().append(i);
//        i2->items().append(i);
//    }

//    FilterTestStub* ft = new FilterTestStub;
//    FilterWorker* fw   = new FilterWorker;
//    ft->setWorkerThread(fw);
//    fw->start();

//    QEventLoop el;

//    QObject::connect(ft, &FilterTestStub::outputChanged, [ft](){
//        QCOMPARE(ft->output()->items().size(), 5);
//        QCOMPARE(ft->output()->items()[0], 2);
//        QCOMPARE(ft->output()->items()[1], 4);
//        QCOMPARE(ft->output()->items()[2], 6);
//        QCOMPARE(ft->output()->items()[3], 8);
//        QCOMPARE(ft->output()->items()[4], 10);
//    });
//    QObject::connect(ft, &FilterTestStub::outputChanged, &el, &QEventLoop::quit);

//    ft->setInput1(i1);
//    ft->setInput2(i2);

//    el.exec();

//    delete i1;
//    delete i2;
//    delete ft;
//    delete fw;
}

void FilterTest::testOneProducerTwoFilters(){
//    SharedDataTestStub* i1 = new SharedDataTestStub;
//    SharedDataTestStub* i2 = new SharedDataTestStub;
//    for ( int i = 1; i <= 5; ++i ){
//        i1->items().append(i);
//        i2->items().append(i);
//    }

//    FilterTestStub* ft = new FilterTestStub;
//    FilterWorker* fw   = new FilterWorker;
//    ft->setWorkerThread(fw);
//    fw->start();

//    QEventLoop el;

//    QObject::connect(ft, &FilterTestStub::outputChanged, [ft](){
//        QCOMPARE(ft->output()->items().size(), 5);
//        QCOMPARE(ft->output()->items()[0], 2);
//        QCOMPARE(ft->output()->items()[1], 4);
//        QCOMPARE(ft->output()->items()[2], 6);
//        QCOMPARE(ft->output()->items()[3], 8);
//        QCOMPARE(ft->output()->items()[4], 10);
//    });
//    QObject::connect(ft, &FilterTestStub::outputChanged, &el, &QEventLoop::quit);

//    ft->setInput1(i1);
//    ft->setInput2(i2);

//    el.exec();
}
