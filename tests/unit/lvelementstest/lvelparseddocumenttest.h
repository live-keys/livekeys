#ifndef LVELPARSEDDOCUMENTTEST_H
#define LVELPARSEDDOCUMENTTEST_H

#include <QObject>
#include "testrunner.h"
#include "live/lockedfileiosession.h"
#include "live/elements/compiler/languageparser.h"

class LvElParsedDocumentTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE
public:
    explicit LvElParsedDocumentTest(QObject *parent = 0);
    ~LvElParsedDocumentTest();
private slots:
    void initTestCase();

    void extractImports1Test();
    void extractImports2Test();
    void extractImports3Test();
    void extractImports4Test();

    void extractInfo1Test();
    void extractInfo2Test();
    void extractInfo3Test();
    void extractInfo4Test();

    void cursorContext1Test();
    void cursorContext2Test();
    void cursorContext3Test();
    void cursorContext4Test();
    void cursorContext5Test();
    void cursorContext6Test();
    void cursorContext7Test();
    void cursorContext8Test();
    void cursorContext9Test();
    void cursorContext10Test();
    void cursorContext11Test();
    void cursorContext12Test();
    void cursorContext13Test();

private:
    lv::el::LanguageParser::Ptr m_parser;
};

#endif // LVELPARSEDDOCUMENTTEST_H
