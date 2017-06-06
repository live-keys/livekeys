#ifndef QENGINEMONITOR_H
#define QENGINEMONITOR_H

#include <QObject>
#include "qliveglobal.h"

class QQuickItem;

namespace lcv{

class Q_LIVE_EXPORT QEngineMonitor : public QObject{

    Q_OBJECT

public:
    explicit QEngineMonitor(QObject *parent = 0);

    static QEngineMonitor* grabFromContext(QQuickItem* item, const QString& contextProperty = "engineMonitor");

signals:
    void beforeCompile();
    void afterCompile();
    void targetChanged();

public slots:
    void emitBeforeCompile();
    void emitAfterCompile();
    void emitTargetChanged();

};

inline void QEngineMonitor::emitBeforeCompile(){
    emit beforeCompile();
}

inline void QEngineMonitor::emitAfterCompile(){
    emit afterCompile();
}

inline void QEngineMonitor::emitTargetChanged(){
    emit targetChanged();
}

}// namespace

#endif // QENGINEMONITOR_H
