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

#include "datetimetest.h"
#include "live/visuallog.h"
#include "live/datetime.h"


Q_TEST_RUNNER_REGISTER(DateTimeTest);

using namespace lv;

DateTimeTest::DateTimeTest(QObject *parent)
    : QObject(parent)
{
}

void DateTimeTest::initTestCase(){
}

void DateTimeTest::testCreation(){
    DateTime dt(2002, 2, 3, 4, 5, 6, 7);
    QVERIFY(dt.year() == 2002);
    QVERIFY(dt.month() == 2);
    QVERIFY(dt.day() == 3);
    QVERIFY(dt.hour() == 4);
    QVERIFY(dt.minute() == 5);
    QVERIFY(dt.second() == 6);
    QVERIFY(dt.msecond() == 7);

    QVERIFY(dt.isAM());
    QVERIFY(!dt.isPM());
    QVERIFY(dt.hourAMPM() == 4);

    DateTime dt2(2002, 2, 4, 15, 5, 6, 7);
    QVERIFY(dt2.hour() == 15);
    QVERIFY(!dt2.isAM());
    QVERIFY(dt2.isPM());
    QVERIFY(dt2.hourAMPM() == 3);
    QVERIFY(dt2.dayOfWeek() == 1);

    QVERIFY(DateTime(2000, 1, 1).dayOfYear() == 1);
    QVERIFY(DateTime(2000, 1, 2).dayOfYear() == 2);
    QVERIFY(DateTime(2000, 2, 1).dayOfYear() == 32);
}

void DateTimeTest::testToString(){
    DateTime dt(2002, 2, 3, 4, 5, 6, 7);
    QVERIFY(dt.format("%Y-%m-%d %H:%M:%S.%i") == "2002-02-03 04:05:06.007");
    QVERIFY(dt.format("%e-%f-%n-%o-%y") == "3- 3-2- 2-02");
    QVERIFY(dt.format("%a-%A-%s-%c") == "am-AM-06.007-0");
    QVERIFY(dt.format("%x") == "x");
    QVERIFY(dt.format("%%") == "%");

    DateTime dt2(2002, 2, 3, 14, 15, 6, 7);
    QVERIFY(dt2.format("%I:%M%A") == "02:15PM");
    QVERIFY(dt2.format("%w-%W-%b-%B") == "Sun-Sunday-Feb-February");
}

void DateTimeTest::testInvalidDate(){
    bool hadError = false;
    try{
        DateTime dt = DateTime::create(2000, 0, 0);
    } catch ( lv::Exception& e ){
        QVERIFY(e.code() == Exception::toCode("~Date"));
        hadError = true;
    }
    hadError = false;
    try{
        DateTime dt = DateTime::create(2000, 1, 1, 100);
    } catch ( lv::Exception& e ){
        QVERIFY(e.code() == Exception::toCode("~Time"));
        hadError = true;
    }
    QVERIFY(hadError);
}

void DateTimeTest::testComparison(){
    QVERIFY(DateTime(2002, 2, 3, 4, 5, 6, 7) == DateTime(2002, 2, 3, 4, 5, 6, 7));
    QVERIFY(DateTime(2002, 2, 3, 4, 5, 6, 6) != DateTime(2002, 2, 3, 4, 5, 6, 7));
    QVERIFY(DateTime(2002, 2, 3, 4, 5, 6, 6) < DateTime(2002, 2, 3, 4, 5, 6, 7));
    QVERIFY(DateTime(2002, 2, 3, 4, 5, 5, 7) < DateTime(2002, 2, 3, 4, 5, 6, 7));
    QVERIFY(DateTime(2002, 2, 3, 4, 4, 6, 7) <= DateTime(2002, 2, 3, 4, 5, 6, 7));
    QVERIFY(DateTime(2002, 2, 3, 3, 5, 6, 7) < DateTime(2002, 2, 3, 4, 5, 6, 7));
    QVERIFY(DateTime(2001, 2, 3) <= DateTime(2002, 2, 3));
    QVERIFY(DateTime(2002, 3, 3) > DateTime(2002, 2, 3));
    QVERIFY(DateTime(2002, 3, 4) >= DateTime(2002, 2, 3));
    QVERIFY(DateTime(2002, 2, 3) >= DateTime(2002, 2, 3));
}

void DateTimeTest::testOperations(){
    DateTime dt(2002, 1, 1);
    QVERIFY(dt.addDays(15) == DateTime(2002, 1, 16));
    QVERIFY(dt.addDays(31) == DateTime(2002, 2, 1));

    DateTime dt2(2002, 1, 1, 15, 0, 0, 0);
    QVERIFY(dt2.addMSeconds(100) == DateTime(2002, 1, 1, 15, 0, 0, 100));
    QVERIFY(dt2.addMSeconds(1000) == DateTime(2002, 1, 1, 15, 0, 1, 0));
    QVERIFY(dt2.addMSeconds(1000 * 60) == DateTime(2002, 1, 1, 15, 1, 0, 0));
    QVERIFY(dt2.addMSeconds(1000 * 60 * 60) == DateTime(2002, 1, 1, 16, 0, 0, 0));
    QVERIFY(dt2.addMSeconds(1000 * 60 * 60 * 10) == DateTime(2002, 1, 2, 1, 0, 0, 0));
}

