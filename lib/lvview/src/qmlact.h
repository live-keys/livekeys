#ifndef LVACTFN_H
#define LVACTFN_H

#include "live/lvviewglobal.h"
#include "live/shared.h"
#include <functional>

#include <QObject>
#include <QVariant>
#include <QQmlParserStatus>

namespace lv{

class WorkerThread;

/// \private
class LV_VIEW_EXPORT QmlAct : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QJSValue run    READ run     WRITE setRun     NOTIFY runChanged)
    Q_PROPERTY(QJSValue args   READ args    WRITE setArgs    NOTIFY argsChanged)
    Q_PROPERTY(QString returns READ returns WRITE setReturns NOTIFY returnsChanged)
    Q_PROPERTY(QJSValue result READ result  NOTIFY resultChanged)

public:
    QmlAct(QObject* parent = nullptr);
    virtual ~QmlAct() override;

    bool isComponentComplete();

    const QJSValue& result() const;

    virtual void process(){}

    void setWorkerThread(WorkerThread* worker);
    WorkerThread* workerThread();

    void setRun(QJSValue run);
    void setArgs(QJSValue args);
    const QJSValue& args() const;
    const QJSValue& run() const;

    void setResult(const QJSValue& result);
    void setResult(const QVariant& result);

    QString returns() const;
    void setReturns(QString returns);

public slots:
    void exec();    

signals:
    void complete();
    void runChanged();
    void resultChanged();
    void argsChanged();

    void returnsChanged(QString returns);

protected:
    void classBegin() override{}
    virtual void componentComplete() override;


private:
    bool     m_isComponentComplete;
    QJSValue m_result;
    QJSValue m_run;
    QJSValue m_args;
    QString  m_returns;
    QList<QPair<int, QQmlProperty>> m_argBindings;
    QJSValueList m_argList;

    WorkerThread* m_workerThread;
};

inline bool QmlAct::isComponentComplete(){
    return m_isComponentComplete;
}

inline const QJSValue &QmlAct::result() const{
    return m_result;
}

inline void QmlAct::setWorkerThread(WorkerThread *worker){
    m_workerThread = worker;
}

inline WorkerThread *QmlAct::workerThread(){
    return m_workerThread;
}

inline const QJSValue& QmlAct::args() const{
    return m_args;
}

inline const QJSValue& QmlAct::run() const{
    return m_run;
}

inline QString QmlAct::returns() const{
    return m_returns;
}

} // namespace

#endif // LVACTFN_H
