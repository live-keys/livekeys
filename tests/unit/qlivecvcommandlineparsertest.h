#ifndef QCOMMANDLINEPARSERTEST_H
#define QCOMMANDLINEPARSERTEST_H

#include <QObject>
#include "qtestrunner.h"

class QLiveCVCommandLineParserTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    QLiveCVCommandLineParserTest(QObject* parent = 0);
    ~QLiveCVCommandLineParserTest();

private slots:
    void initTestCase();
    void versionFlagTest();
    void helpFlagTest();
    void noFlagTest();
    void flagMultiNameTest();
    void noOptionTest();
    void optionTest();
    void optionAndFlagTest();
    void invalidOptiontest();

    void scriptTest();

    void scriptNoFlagTest();
    void scriptFlagMultiNameTest();
    void scriptNoOptionTests();
    void scriptOptionTest();
    void scriptOptionAndFlagTest();
    void scriptInvalidOptionTest();

    //TODO
//    void scriptHelpFlagTest();
};

#endif // QCOMMANDLINEPARSERTEST_H
