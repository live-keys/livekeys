#ifndef LVPARSEERRORTEST_H
#define LVPARSEERRORTEST_H

#include <QObject>
#include "testrunner.h"
#include "live/lockedfileiosession.h"

class LvParseErrorTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    explicit LvParseErrorTest(QObject *parent = nullptr);
    ~LvParseErrorTest(){}

private slots:
    void initTestCase();
    void programBodyExtraElements();
    void identifierNotFound();

private:
    lv::LockedFileIOSession::Ptr m_fileSession;
    std::string                  m_scriptPath;
};

#endif // LVPARSEERRORTEST_H
