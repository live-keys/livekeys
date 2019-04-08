#ifndef LINECAPTURETEST_H
#define LINECAPTURETEST_H

#include <QObject>
#include "testrunner.h"

class LineCaptureTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit LineCaptureTest(QObject *parent = nullptr);

private slots:
    void initTestCase();

    void singleMessageTest();
    void partialSingleMessageTest();
    void partialLineSplitSingleMessageTest();
    void multiMessageTest();
    void partialMultiMessageTest();

    void mixedSymbolsTest();

    void parseTypeErrorTest();
    void parseLossDataWarningTest();
    void parseLengthErrorTest();
    void parseTypeNumberErrorTest();
    void parseLenNumberErrorTest();
};

#endif // LINECAPTURETEST_H
