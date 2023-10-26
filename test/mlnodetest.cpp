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
#include "live/visuallog.h"

using namespace lv;


TEST_CASE( "MLNode Test", "[MLNode]" ) {
    SECTION("Test Constructor"){
        REQUIRE(MLNode().type() == MLNode::Null);
        REQUIRE(MLNode("str").type() == MLNode::String);
        REQUIRE(MLNode(MLNode::StringType()).type() == MLNode::String);
        REQUIRE(MLNode(200).type() == MLNode::Integer);
        REQUIRE(MLNode(200.2).type() == MLNode::Float);
        REQUIRE(MLNode(true).type() == MLNode::Boolean);
        REQUIRE(MLNode(MLNode::BytesType()).type() == MLNode::Bytes);
        REQUIRE(MLNode(MLNode::ObjectType()).type() == MLNode::Object);
        REQUIRE(MLNode(MLNode::ArrayType()).type() == MLNode::Array);
    }
    SECTION("Test Constructor From Initializer List"){
        MLNode n({100, 200, 300});
        REQUIRE(n.type() == MLNode::Array);

        MLNode nObject({{"object", "value"}});
        REQUIRE(nObject.type() == MLNode::Object);
    }
    SECTION("Test Assignment"){
        MLNode n = 100;
        REQUIRE(n.type() == MLNode::Integer);
        n = 100.20;
        REQUIRE(n.type() == MLNode::Float);

        n = {100, 200, 300};
        REQUIRE(n.type() == MLNode::Array);

        n = {{"object", "value"}};
        REQUIRE(n.type() == MLNode::Object);

        char* data = new char[10];
        for ( unsigned char i = 0; i < 10; ++i )
            data[i] = i;
        n = MLNode::BytesType(data, 10);
        REQUIRE(n.type() == MLNode::Bytes);
        delete[] data;
    }
    SECTION("Test Access Operator"){
        MLNode n = 100;
        REQUIRE_THROWS_AS(n[100],   InvalidMLTypeException);
        REQUIRE_THROWS_AS(n["200"], InvalidMLTypeException);

        n = {100, 200, 300};
        REQUIRE(n[0].asInt() == 100);
        REQUIRE(n[1].asInt() == 200);
        REQUIRE(n[2].asInt() == 300);
        REQUIRE_THROWS_AS(n["200"], InvalidMLTypeException);

        n = {{"key", "value"}};
        REQUIRE_THROWS_AS(n[100], InvalidMLTypeException);
        REQUIRE(n["key"].asString() == MLNode::StringType("value"));

        n = {
            {"object", {
                 {"key1", "value1"},
                 {"key2", 100}
            }},
            {"array", { 100, "200", false}},
            {"bool", true},
            {"int", 100},
            {"float", 100.1}
        };

        REQUIRE(n["object"]["key1"].asString() == MLNode::StringType("value1"));
        REQUIRE(n["object"]["key2"].asInt() == 100);
        REQUIRE(n["array"][0].asInt() == 100);
        REQUIRE(n["array"][1].asString() == MLNode::StringType("200"));
        REQUIRE(n["array"][2].asBool() == false);
        REQUIRE(n["bool"].asBool() == true);
        REQUIRE(n["int"].asInt() == 100);
        REQUIRE(n["float"].asFloat() == 100.1);
    }
    SECTION("Test Base64 To Bytes"){
        const char* str = "!@(^$#*(@$!:";
        ByteBuffer base64 = ByteBuffer::encodeBase64(ByteBuffer(str, 12));

        MLNode n = MLNode::StringType(base64.data());
        REQUIRE(n.type() == MLNode::String);

        MLNode::BytesType roundtrip = n.asBytes();
        REQUIRE(MLNode::StringType(reinterpret_cast<const char*>(roundtrip.data()), roundtrip.size()) == MLNode::StringType(str));
    }
    SECTION("Test Iterator"){
        MLNode n;
        n = MLNode::ArrayType();
        REQUIRE(n.begin() == n.end());


        n = {100, 200, 300};
        MLNode::Iterator it = n.begin();
        REQUIRE(it.value().asInt() == 100);
        REQUIRE(it < n.end());
        ++it;
        REQUIRE(it->asInt() == 200);
        ++it;
        REQUIRE(it->asInt() == 300);
        ++it;
        REQUIRE(it == n.end());

        it = n.begin() + 1;
        REQUIRE(it->asInt() == 200);
        it += 2;
        REQUIRE(it == n.end());
        --it;
        REQUIRE(it->asInt() == 300);
        it -= 2;
        REQUIRE(it->asInt() == 100);
        REQUIRE(it == n.begin());


        n = 100;
        it = n.begin();
        REQUIRE(it.value().asInt() == 100);
        REQUIRE(it + 1 == n.end());

        n = {
            {"a", {
                 {"key1", "value1"},
                 {"key2", 100}
            }},
            {"b", { 100, "200", false}},
            {"c", true},
            {"d", 100},
            {"e", 100.1}
        };

        it = n.begin();
        REQUIRE_THROWS_AS(n.begin() + 3, InvalidMLTypeException);

        REQUIRE(it.key() == MLNode::StringType("a"));
        REQUIRE_THROWS_AS(it += 3, InvalidMLTypeException);
        ++it;
        REQUIRE(it.key() == MLNode::StringType("b"));
        REQUIRE(it.value().begin()->asInt() == 100);
        ++it;
        REQUIRE(it.key() == MLNode::StringType("c"));
        REQUIRE(it.value().asBool() == true);
        ++it;
        REQUIRE(it.key() == MLNode::StringType("d"));
        REQUIRE(it->asInt() == 100);
        --it;
        REQUIRE(it.key() == MLNode::StringType("c"));
        ++it;
        ++it;
        ++it;
        REQUIRE(it == n.end());
    }
    SECTION("Test Const Iterator"){
        MLNode n;

        n = {100, 200, 300};
        MLNode::ConstIterator it = n.begin();
        REQUIRE(it.value().asInt() == 100);
        REQUIRE(it < n.end());
        ++it;
        REQUIRE(it->asInt() == 200);
        ++it;
        REQUIRE(it->asInt() == 300);
        ++it;
        REQUIRE(it == n.cend());

        it = n.cbegin() + 1;
        REQUIRE(it->asInt() == 200);
        it += 2;
        REQUIRE(it == n.cend());
        --it;
        REQUIRE(it->asInt() == 300);
        it -= 2;
        REQUIRE(it->asInt() == 100);
        REQUIRE(it == n.cbegin());


        n = 100;
        it = n.cbegin();
        REQUIRE(it.value().asInt() == 100);
        REQUIRE(it + 1 == n.cend());

        n = {
            {"a", {
                 {"key1", "value1"},
                 {"key2", 100}
            }},
            {"b", { 100, "200", false}},
            {"c", true},
            {"d", 100},
            {"e", 100.1}
        };

        it = n.cbegin();
        REQUIRE_THROWS_AS(n.begin() + 3, InvalidMLTypeException);

        REQUIRE(it.key() == MLNode::StringType("a"));
        REQUIRE_THROWS_AS(it += 3, InvalidMLTypeException);
        ++it;
        REQUIRE(it.key() == MLNode::StringType("b"));
        REQUIRE(it.value().begin()->asInt() == 100);
        ++it;
        REQUIRE(it.key() == MLNode::StringType("c"));
        REQUIRE(it.value().asBool() == true);
        ++it;
        REQUIRE(it.key() == MLNode::StringType("d"));
        REQUIRE(it->asInt() == 100);
        --it;
        REQUIRE(it.key() == MLNode::StringType("c"));
        ++it;
        ++it;
        ++it;
        REQUIRE(it == n.cend());
    }
}
