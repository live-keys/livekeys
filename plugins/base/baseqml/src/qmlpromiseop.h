#ifndef LVQMLPROMISEOP_H
#define LVQMLPROMISEOP_H

#include <QObject>
#include "live/viewengine.h"
#include "live/qmlpromise.h"

namespace lv{

class QmlPromiseOp : public QObject{

    Q_OBJECT

public:
    explicit QmlPromiseOp(QQmlEngine* engine, QObject *parent = nullptr);

public slots:
    lv::QmlPromise* create(QJSValue fn);
    lv::QmlPromise* resolve(QJSValue val);
    lv::QmlPromise* reject(QJSValue val);

private:
    ViewEngine* m_engine;
};

}// namespace

#endif // LVQMLPROMISEOP_H
