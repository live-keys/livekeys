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

#include "catch_library.h"
#include "live/mlnode.h"
#include "live/mlnodetojson.h"
#include "live/visuallog.h"

using namespace lv;

TEST_CASE( "MLNode to Json Test", "[MLNodeToJson]" ){
    SECTION("Test Serialize"){
        MLNode n = {
            {"object", {
                 {"string", "value1"},
                 {"key2", 100}
            }},
            {"array", { 100, "200", false}},
            {"bool", true},
            {"int", 100},
            {"float", 100.1},
            {"null", nullptr}
        };

        std::string serialized;
        ml::toJson(n, serialized);

        REQUIRE(serialized == "{\"array\":[100,\"200\",false],\"bool\":true,\"float\":100.1,\"int\""
                              ":100,\"null\":null,\"object\":{\"key2\":100,\"string\":\"value1\"}}");

    }
    SECTION("Test Deserialize"){
        MLNode n = {
            {"object", {
                 {"string", "value1"},
                 {"key2", 100}
            }},
            {"array", { 100, "200", false}},
            {"bool", true},
            {"int", 100},
            {"float", 100.1},
            {"null", nullptr}
        };

        std::string serialized;
        ml::toJson(n, serialized);

        MLNode rt;
        ml::fromJson(serialized, rt);

        REQUIRE(rt.type() == MLNode::Type::Object);
        REQUIRE(rt["object"].type() == MLNode::Type::Object);
        REQUIRE(rt["object"].size() == 2);
        REQUIRE(rt["object"]["string"].asString() == MLNode::StringType("value1"));
        REQUIRE(rt["object"]["key2"].asInt() == 100);

        REQUIRE(rt["array"].size() == 3);
        REQUIRE(rt["array"][0].asInt() == 100);
        REQUIRE(rt["array"][1].asString() == MLNode::StringType("200"));
        REQUIRE(rt["array"][2].asBool() == false);

        REQUIRE(rt["bool"].asBool() == true);
        REQUIRE(rt["int"].asInt() == 100);
        REQUIRE(rt["float"].asFloat() == 100.1);
        REQUIRE(rt["null"].isNull());
    }
}

