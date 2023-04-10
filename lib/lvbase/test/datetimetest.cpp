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

#include "catch_amalgamated.hpp"
#include "live/visuallog.h"
#include "live/datetime.h"

using namespace lv;

TEST_CASE( "DateTime Test", "[DateTime]" ) {
    SECTION("Test Creation"){
        DateTime dt(2002, 2, 3, 4, 5, 6, 7);
        REQUIRE(dt.year() == 2002);
        REQUIRE(dt.month() == 2);
        REQUIRE(dt.day() == 3);
        REQUIRE(dt.hour() == 4);
        REQUIRE(dt.minute() == 5);
        REQUIRE(dt.second() == 6);
        REQUIRE(dt.msecond() == 7);

        REQUIRE(dt.isAM());
        REQUIRE(!dt.isPM());
        REQUIRE(dt.hourAMPM() == 4);

        DateTime dt2(2002, 2, 4, 15, 5, 6, 7);
        REQUIRE(dt2.hour() == 15);
        REQUIRE(!dt2.isAM());
        REQUIRE(dt2.isPM());
        REQUIRE(dt2.hourAMPM() == 3);
        REQUIRE(dt2.dayOfWeek() == 1);

        REQUIRE(DateTime(2000, 1, 1).dayOfYear() == 1);
        REQUIRE(DateTime(2000, 1, 2).dayOfYear() == 2);
        REQUIRE(DateTime(2000, 2, 1).dayOfYear() == 32);
    }
    SECTION("Test toString"){
        DateTime dt(2002, 2, 3, 4, 5, 6, 7);
        REQUIRE(dt.format("%Y-%m-%d %H:%M:%S.%i") == "2002-02-03 04:05:06.007");
        REQUIRE(dt.format("%e-%f-%n-%o-%y") == "3- 3-2- 2-02");
        REQUIRE(dt.format("%a-%A-%s-%c") == "am-AM-06.007-0");
        REQUIRE(dt.format("%x") == "x");
        REQUIRE(dt.format("%%") == "%");

        DateTime dt2(2002, 2, 3, 14, 15, 6, 7);
        REQUIRE(dt2.format("%I:%M%A") == "02:15PM");
        REQUIRE(dt2.format("%w-%W-%b-%B") == "Sun-Sunday-Feb-February");
    }
    SECTION("Test InvalidDate"){
        bool hadError = false;
        try{
            DateTime dt = DateTime::create(2000, 0, 0);
        } catch ( lv::Exception& e ){
            REQUIRE(e.code() == Exception::toCode("~Date"));
            hadError = true;
        }
        hadError = false;

        try{
            DateTime dt = DateTime::create(2000, 1, 1, 100);
        } catch ( lv::Exception& e ){
            REQUIRE(e.code() == Exception::toCode("~Time"));
            hadError = true;
        }
        REQUIRE(hadError);
    }
    SECTION("Test Comparison"){
        REQUIRE(DateTime(2002, 2, 3, 4, 5, 6, 7) == DateTime(2002, 2, 3, 4, 5, 6, 7));
        REQUIRE(DateTime(2002, 2, 3, 4, 5, 6, 6) != DateTime(2002, 2, 3, 4, 5, 6, 7));
        REQUIRE(DateTime(2002, 2, 3, 4, 5, 6, 6) < DateTime(2002, 2, 3, 4, 5, 6, 7));
        REQUIRE(DateTime(2002, 2, 3, 4, 5, 5, 7) < DateTime(2002, 2, 3, 4, 5, 6, 7));
        REQUIRE(DateTime(2002, 2, 3, 4, 4, 6, 7) <= DateTime(2002, 2, 3, 4, 5, 6, 7));
        REQUIRE(DateTime(2002, 2, 3, 3, 5, 6, 7) < DateTime(2002, 2, 3, 4, 5, 6, 7));
        REQUIRE(DateTime(2001, 2, 3) <= DateTime(2002, 2, 3));
        REQUIRE(DateTime(2002, 3, 3) > DateTime(2002, 2, 3));
        REQUIRE(DateTime(2002, 3, 4) >= DateTime(2002, 2, 3));
        REQUIRE(DateTime(2002, 2, 3) >= DateTime(2002, 2, 3));
    }
    SECTION("Test Operations"){
        DateTime dt(2002, 1, 1);
        REQUIRE(dt.addDays(15) == DateTime(2002, 1, 16));
        REQUIRE(dt.addDays(31) == DateTime(2002, 2, 1));

        DateTime dt2(2002, 1, 1, 15, 0, 0, 0);
        REQUIRE(dt2.addMSeconds(100) == DateTime(2002, 1, 1, 15, 0, 0, 100));
        REQUIRE(dt2.addMSeconds(1000) == DateTime(2002, 1, 1, 15, 0, 1, 0));
        REQUIRE(dt2.addMSeconds(1000 * 60) == DateTime(2002, 1, 1, 15, 1, 0, 0));
        REQUIRE(dt2.addMSeconds(1000 * 60 * 60) == DateTime(2002, 1, 1, 16, 0, 0, 0));
        REQUIRE(dt2.addMSeconds(1000 * 60 * 60 * 10) == DateTime(2002, 1, 2, 1, 0, 0, 0));
    }
}
