#include "linecapturetest.h"
#include "live/linecapture.h"
#include "live/linemessage.h"

Q_TEST_RUNNER_REGISTER(LineCaptureTest);

using namespace lv;

class LineCaptureStub{

public:
    LineCaptureStub(LineCapture& lc){
        lc.onMessage([this](const LineMessage& message, void*){
            messages.push_back(message);
        });
        lc.onError([this](int type, const std::string&){
            errors.push_back(type);
        });
    }

    std::vector<LineMessage> messages;
    std::vector<int>         errors;
};


LineCaptureTest::LineCaptureTest(QObject *parent)
    : QObject(parent)
{
}

void LineCaptureTest::initTestCase(){
}

void LineCaptureTest::singleMessageTest(){
    LineCapture lc;

    LineCaptureStub captures(lc);

    lc.append("Type:1;Len:10\r\n1234567890");


    QVERIFY(captures.messages.size() == 1);
    QVERIFY(captures.messages[0].type == 1);
    QVERIFY(captures.messages[0].data == "1234567890");
}

void LineCaptureTest::partialSingleMessageTest(){
    LineCapture lc;

    LineCaptureStub captures(lc);

    lc.append("Type:1;Len:10");
    lc.append("\r\n1234");

    QVERIFY(captures.messages.size() == 0);

    lc.append("567890");

    QVERIFY(captures.messages.size() == 1);
    QVERIFY(captures.messages[0].type == 1);
    QVERIFY(captures.messages[0].data == "1234567890");
}

void LineCaptureTest::partialLineSplitSingleMessageTest(){
    LineCapture lc;

    LineCaptureStub captures(lc);

    lc.append("Type:1;Len:10\r");
    lc.append("\n1234");

    QVERIFY(captures.messages.size() == 0);

    lc.append("567890");

    QVERIFY(captures.messages.size() == 1);
    QVERIFY(captures.messages[0].type == 1);
    QVERIFY(captures.messages[0].data == "1234567890");
}


void LineCaptureTest::multiMessageTest(){
    LineCapture lc;

    LineCaptureStub captures(lc);

    lc.append("Type:1;Len:10\r\n1234567890Type:1;Len:10\r\n1234567890Type:1;Len:10\r\n1234567890");


    QVERIFY(captures.messages.size() == 3);
    QVERIFY(captures.messages[0].type == 1);
    QVERIFY(captures.messages[0].data == "1234567890");
    QVERIFY(captures.messages[1].data == "1234567890");
    QVERIFY(captures.messages[2].data == "1234567890");
}

void LineCaptureTest::partialMultiMessageTest(){
    LineCapture lc;

    LineCaptureStub captures(lc);

    lc.append("Type:1;Len:10\r");
    lc.append("\n1234567");
    lc.append("890Type:1;Len:10\r\n1234567890");
    lc.append("Type:1;Len:10\r\n123456789");
    lc.append("0");


    QVERIFY(captures.messages.size() == 3);
    QVERIFY(captures.messages[0].type == 1);
    QVERIFY(captures.messages[0].data == "1234567890");
    QVERIFY(captures.messages[1].data == "1234567890");
    QVERIFY(captures.messages[2].data == "1234567890");
}

void LineCaptureTest::mixedSymbolsTest(){
    LineCapture lc;

    LineCaptureStub captures(lc);

    lc.append("Type:1;Len:10\r");
    lc.append("\n1234567");
    lc.append("8\r\nType:1;Len:10\r\n1234567890");
    lc.append("Type:1;Len:10\r\n\r\n3456789");
    lc.append("0");

    QVERIFY(captures.messages.size() == 3);
    QVERIFY(captures.messages[0].type == 1);
    QVERIFY(captures.messages[0].data == "12345678\r\n");
    QVERIFY(captures.messages[1].data == "1234567890");
    QVERIFY(captures.messages[2].data == "\r\n34567890");
}

void LineCaptureTest::parseTypeErrorTest(){
    LineCapture lc;

    LineCaptureStub captures(lc);

    lc.append(":1;Len:10\r");
    lc.append("\nType:1;Len:10\r\n1234567890");
    lc.append("Type:1;Len:10\r\n123456789");
    lc.append("0");

    QVERIFY(captures.errors.size() == 1);
    QVERIFY(captures.errors[0] == LineCapture::ParseTypeError);

    QVERIFY(captures.messages.size() == 2);
    QVERIFY(captures.messages[0].data == "1234567890");
    QVERIFY(captures.messages[1].data == "1234567890");
}

void LineCaptureTest::parseLossDataWarningTest(){
    LineCapture lc;

    LineCaptureStub captures(lc);

    lc.append("0123Type:1;Len:10\r\n1234567890");
    lc.append("Type:1;Len:10\r\n123456789");
    lc.append("0");

    QVERIFY(captures.errors.size() == 1);
    QVERIFY(captures.errors[0] == LineCapture::ParseLossData);

    QVERIFY(captures.messages.size() == 2);
    QVERIFY(captures.messages[0].data == "1234567890");
    QVERIFY(captures.messages[1].data == "1234567890");
}

void LineCaptureTest::parseLengthErrorTest(){
    LineCapture lc;

    LineCaptureStub captures(lc);

    lc.append("Type:1;Le:10\r");
    lc.append("\nType:1;Len:10\r\n1234567890");
    lc.append("Type:1;Len:10\r\n123456789");
    lc.append("0");

    QVERIFY(captures.errors.size() == 1);
    QVERIFY(captures.errors[0] == LineCapture::ParseLengthError);

    QVERIFY(captures.messages.size() == 2);
    QVERIFY(captures.messages[0].data == "1234567890");
    QVERIFY(captures.messages[1].data == "1234567890");
}

void LineCaptureTest::parseTypeNumberErrorTest(){
    LineCapture lc;

    LineCaptureStub captures(lc);

    lc.append("Type:1a;Len:10\r");
    lc.append("\nType:1;Len:10\r\n1234567890");
    lc.append("Type:1;Len:10\r\n123456789");
    lc.append("0");

    QVERIFY(captures.errors.size() == 1);
    QVERIFY(captures.errors[0] == LineCapture::TypeNumberError);

    QVERIFY(captures.messages.size() == 2);
    QVERIFY(captures.messages[0].data == "1234567890");
    QVERIFY(captures.messages[1].data == "1234567890");
}

void LineCaptureTest::parseLenNumberErrorTest(){
    LineCapture lc;

    LineCaptureStub captures(lc);

    lc.append("Type:1;Len:1a0\r");
    lc.append("\nType:1;Len:10\r\n1234567890");
    lc.append("Type:1;Len:10\r\n123456789");
    lc.append("0");

    QVERIFY(captures.errors.size() == 1);
    QVERIFY(captures.errors[0] == LineCapture::LengthNumberError);

    QVERIFY(captures.messages.size() == 2);
    QVERIFY(captures.messages[0].data == "1234567890");
    QVERIFY(captures.messages[1].data == "1234567890");
}
