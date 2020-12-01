#ifndef LVQMLWORKERINTERFACE_H
#define LVQMLWORKERINTERFACE_H

#include "live/lvbaseqmlglobal.h"
#include "live/qmlworkerpool.h"
#include "live/shared.h"

#include <QObject>
#include <QList>

namespace lv{

class LV_BASEQML_EXPORT QmlWorkerInterface : public QObject{

    Q_OBJECT

public:
    explicit QmlWorkerInterface(QObject *parent = nullptr);

    virtual void start(QmlWorkerPool::Task* task, int priority = 9);

};

}// namespace

#endif // LVQMLWORKERINTERFACE_H
