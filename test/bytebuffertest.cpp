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
#include "live/visuallog.h"
#include "live/datetime.h"

using namespace lv;

TEST_CASE( "ByteBuffer Test", "[ByteBuffer]" ) {
    SECTION("Test Encode"){
        char s[] = "";
        ByteBuffer bf(s, 0);
        REQUIRE(ByteBuffer::encodeBase64(bf).size() == 0);

        char s2[] = "f";
        char s2expect[] = "Zg==";
        ByteBuffer bf2(s2, 1);
        ByteBuffer encoded = ByteBuffer::encodeBase64(bf2);
        REQUIRE(encoded.size() == 4);
        for ( size_t i = 0; i < 4; ++i ){
            REQUIRE(encoded.data()[i] == s2expect[i] );
        }
    }
    SECTION("Test Round Trip"){
        char s[] = "x#*$38247308";
        ByteBuffer bf(s, 12);
        ByteBuffer encoded = ByteBuffer::encodeBase64(bf);
        ByteBuffer decoded = ByteBuffer::decodeBase64(encoded);
        REQUIRE(decoded.size() == 12);

        for ( size_t i = 0; i < decoded.size(); ++i ){
            REQUIRE(decoded.data()[i] == s[i]);
        }
    }
}

