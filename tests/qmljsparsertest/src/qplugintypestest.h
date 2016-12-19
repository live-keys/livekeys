#ifndef QPLUGINTYPESTEST_HPP
#define QPLUGINTYPESTEST_HPP

#include <QObject>
#include "qtestrunner.h"

class QPluginTypesTest : public QObject{

    Q_OBJECT
    Q_TEST_RUNNER_SUITE

public:
    QPluginTypesTest(QObject* parent = 0);
    ~QPluginTypesTest();

    QString filePath(const QString& relativePath);
    QByteArray readFile(const QString& path);

private slots:
    void initTestCase();
    void getTypesFromEngine();
};

#endif // QPLUGINTYPESTEST_HPP
