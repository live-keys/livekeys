#ifndef LVQMLACT_H
#define LVQMLACT_H

#include "live/lvbaseqmlglobal.h"
#include "live/shared.h"
#include "live/qmlworkerinterface.h"
#include "live/componentdeclaration.h"

#include <QObject>
#include <QVariant>
#include <QQmlParserStatus>

namespace lv{

/// \private
class LV_BASEQML_EXPORT QmlAct : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_ENUMS(Trigger)
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QJSValue run                   READ run     WRITE setRun     NOTIFY runChanged)
    Q_PROPERTY(QJSValue args                  READ args    WRITE setArgs    NOTIFY argsChanged)
    Q_PROPERTY(QString returns                READ returns WRITE setReturns NOTIFY returnsChanged)
    Q_PROPERTY(QJSValue result                READ result  NOTIFY resultChanged)
    Q_PROPERTY(Trigger trigger                READ trigger WRITE setTrigger NOTIFY triggerChanged)
    Q_PROPERTY(lv::QmlWorkerInterface* worker READ worker  WRITE setWorker NOTIFY workerChanged)

public:
    enum Trigger{
        PropertyChange,
        Manual
    };

    class RunSource{
    public:
        RunSource(ComponentDeclaration cd) : declarationLocation(cd){}

        ComponentDeclaration declarationLocation;
        QByteArray imports;
        QByteArray source;
    };

public:
    QmlAct(QObject* parent = nullptr);
    virtual ~QmlAct() override;

    bool isComponentComplete();

    const QJSValue& result() const;

    virtual void process(){}

    void setRun(QJSValue run);
    const QJSValue& run() const;

    void setArgs(QJSValue args);
    const QJSValue& args() const;

    void setResult(const QJSValue& result);
    void setResult(const QVariant& result);

    QString returns() const;
    void setReturns(QString returns);

    lv::QmlWorkerInterface* worker() const;
    void setWorker(lv::QmlWorkerInterface* worker);

    bool event(QEvent *event) override;

    Trigger trigger() const;
    void setTrigger(Trigger trigger);

    bool isRunning() const;

public slots:
    void exec();

    void __propertyChange();

signals:
    void complete();
    void runChanged();
    void resultChanged();
    void argsChanged();
    void workerChanged();
    void returnsChanged();
    void triggerChanged();

protected:
    void classBegin() override{}
    virtual void componentComplete() override;

private:
    void extractSource(ViewEngine* engine);

    bool     m_isComponentComplete;
    Trigger  m_trigger;
    QJSValue m_result;
    QJSValue m_run;
    QJSValue m_args;
    QString  m_returns;
    QList<QPair<int, QQmlProperty>> m_argBindings;
    QJSValueList m_argList;

    QmlWorkerInterface*  m_worker;
    RunSource*           m_source;
    QmlWorkerPool::QmlFunctionTask* m_currentTask;
    bool                            m_execAfterCurrent;
};

inline bool QmlAct::isComponentComplete(){
    return m_isComponentComplete;
}

inline const QJSValue &QmlAct::result() const{
    return m_result;
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

inline QmlWorkerInterface *QmlAct::worker() const{
    return m_worker;
}

inline QmlAct::Trigger QmlAct::trigger() const{
    return m_trigger;
}

inline void QmlAct::setTrigger(QmlAct::Trigger trigger){
    if (m_trigger == trigger)
        return;

    m_trigger = trigger;
    emit triggerChanged();
}

inline bool QmlAct::isRunning() const{
    return m_currentTask;
}

} // namespace

#endif // LVQMLACT_H
