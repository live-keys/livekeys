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

private slots:
    void initTestCase();
    void identifierTest();

};

#endif // QDOCUMENTQMLPARSERTEST_H
