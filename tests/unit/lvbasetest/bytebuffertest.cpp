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

#include "bytebuffertest.h"
#include "live/visuallog.h"
#include "live/datetime.h"


Q_TEST_RUNNER_REGISTER(ByteBufferTest);

using namespace lv;

ByteBufferTest::ByteBufferTest(QObject *parent)
    : QObject(parent)
{
}

ByteBufferTest::~ByteBufferTest(){
}

void ByteBufferTest::initTestCase(){
}

void ByteBufferTest::testEncode(){
    char s[] = "";
    ByteBuffer bf(s, 0);
    QVERIFY(ByteBuffer::encodeBase64(bf).size() == 0);

    char s2[] = "f";
    char s2expect[] = "Zg==";
    ByteBuffer bf2(s2, 1);
    ByteBuffer encoded = ByteBuffer::encodeBase64(bf2);
    QVERIFY(encoded.size() == 4);
    for ( size_t i = 0; i < 4; ++i ){
        QVERIFY(encoded.data()[i] == s2expect[i] );
    }
}

void ByteBufferTest::testRoundTrip(){
    char s[] = "x#*$38247308";
    ByteBuffer bf(s, 12);
    ByteBuffer encoded = ByteBuffer::encodeBase64(bf);
    ByteBuffer decoded = ByteBuffer::decodeBase64(encoded);
    QVERIFY(decoded.size() == 12);

    for ( size_t i = 0; i < decoded.size(); ++i ){
        QVERIFY(decoded.data()[i] == s[i]);
    }
}
