#ifndef ENGINETESTSTUB_H
#define ENGINETESTSTUB_H

#include <QObject>
#include "live/engine.h"

class EngineTestStub : public QObject{

    Q_OBJECT

public:
    explicit EngineTestStub(QObject *parent = 0);

public slots:
    void throwException();
    void throwJsError();
    void throwJsWarning();
};

#endif // ENGINETESTSTUB_H
