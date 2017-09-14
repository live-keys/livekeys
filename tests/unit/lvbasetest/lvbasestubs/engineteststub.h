#ifndef ENGINETESTSTUB_H
#define ENGINETESTSTUB_H

#include <QObject>

class EngineTestStub : public QObject{

    Q_OBJECT

public:
    explicit EngineTestStub(QObject *parent = 0);

public slots:
    void throwException();
    void throwJsException();
};

#endif // ENGINETESTSTUB_H
