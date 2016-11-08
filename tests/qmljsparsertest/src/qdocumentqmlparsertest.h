#ifndef QDOCUMENTQMLPARSERTEST_H
#define QDOCUMENTQMLPARSERTEST_H

#include <QObject>
#include "qtestrunner.h"

class QDocumentQmlParserTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    QDocumentQmlParserTest(QObject* parent = 0);
    ~QDocumentQmlParserTest();

private:
    QString filePath(const QString& relativePath);
    QString readFile(const QString& path);

    void initTestCase();
    void identifierTest();
    void identifierValueTest();
    void identifierFunctionMemberTest();
    void identifierMemberTest();



private slots:
    void identifierScopeTest();
    void rangeScopeTest();
};

#endif // QDOCUMENTQMLPARSERTEST_H
