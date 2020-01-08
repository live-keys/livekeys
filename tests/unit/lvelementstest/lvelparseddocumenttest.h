#ifndef LVELPARSEDDOCUMENTTEST_H
#define LVELPARSEDDOCUMENTTEST_H

#include <QObject>
#include "testrunner.h"
#include "live/lockedfileiosession.h"
#include "live/elements/languageparser.h"

class LvElParsedDocumentTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE
public:
    explicit LvElParsedDocumentTest(QObject *parent = 0);
    ~LvElParsedDocumentTest();
private slots:
    void initTestCase();

    void testExtractImports1();
    void testExtractImports2();
    void testExtractImports3();
    void testExtractImports4();

    void testExtractInfo1();
    void testExtractInfo2();
    void testExtractInfo3();
    void testExtractInfo4();

    void testCursorContext1();
    void testCursorContext2();
    void testCursorContext3();
    void testCursorContext4();
    void testCursorContext5();
    void testCursorContext6();
    void testCursorContext7();
    void testCursorContext8();
    void testCursorContext9();
    void testCursorContext10();
    void testCursorContext11();
    void testCursorContext12();

private:
    lv::el::LanguageParser::Ptr m_parser;
};

#endif // LVELPARSEDDOCUMENTTEST_H
