#ifndef LVQMLACT_H
#define LVQMLACT_H

#include "live/lvbaseqmlglobal.h"
#include "live/shared.h"
#include "live/qmlworkerinterface.h"
#include "live/componentdeclaration.h"
#include "live/qmlfunctionsource.h"

#include <QObject>
#include <QVariant>
#include <QQmlParserStatus>

namespace lv{


class QmlPromise;
class LV_BASEQML_EXPORT QmlAct : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_ENUMS(Trigger)
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QJSValue runFunction           READ runFunction NOTIFY runFunctionChanged)
    Q_PROPERTY(QJSValue args                  READ args        WRITE setArgs    NOTIFY argsChanged)
    Q_PROPERTY(QJSValue unwrap                READ unwrap      WRITE setUnwrap  NOTIFY unwrapChanged)
    Q_PROPERTY(QJSValue returns               READ returns     WRITE setReturns NOTIFY returnsChanged)
    Q_PROPERTY(QJSValue result                READ result      NOTIFY resultChanged)
    Q_PROPERTY(Trigger trigger                READ trigger     WRITE setTrigger NOTIFY triggerChanged)
    Q_PROPERTY(lv::QmlWorkerInterface* worker READ worker      WRITE setWorker NOTIFY workerChanged)

public:
    enum Trigger{
        PropertyChange,
        Manual
    };

public:
    QmlAct(QObject* parent = nullptr);
    virtual ~QmlAct() override;

    bool isComponentComplete();

    const QJSValue& result() const;

    QJSValue runFunction() const;

    void setArgs(QJSValue args);
    const QJSValue& args() const;

    void setResult(const QJSValue& result);
    void setResult(const QVariant& result);

    QJSValue returns() const;
    void setReturns(QJSValue returns);

    lv::QmlWorkerInterface* worker() const;
    void setWorker(lv::QmlWorkerInterface* worker);

    bool event(QEvent *event) override;

    Trigger trigger() const;
    void setTrigger(Trigger trigger);

    QJSValue unwrap() const;
    void setUnwrap(QJSValue unwrap);

    bool isRunning() const;


    QJSValue apply(const QJSValueList& args);
    const QJSValueList& argumentCallList() const;

    static bool parserPropertyValidateHook(
        QmlSourceLocation location,
        QmlSourceLocation valueLocation,
        const QString& name);
    static void parserPropertyHook(
        ViewEngine* ve,
        QObject* target,
        const QByteArray& imports,
        QmlSourceLocation location,
        QmlSourceLocation valueLocation,
        const QString& name,
        const QString& source
    );

public slots:
    void exec();

    void __propertyChange();

signals:
    void complete();
    void runFunctionChanged();
    void resultChanged();
    void argsChanged();
    void workerChanged();
    void returnsChanged();
    void triggerChanged();
    void unwrapChanged();

protected:
    void classBegin() override{}
    virtual void componentComplete() override;

private:
    bool                            m_isComponentComplete;
    Trigger                         m_trigger;
    QJSValue                        m_result;
    QJSValue                        m_args;
    QJSValue                        m_returns;
    QmlFunctionSource*              m_runSource;
    QList<QPair<int, QQmlProperty>> m_argBindings;
    QJSValueList                    m_argList;

    QmlPromise*                     m_promiseResult;
    QmlWorkerInterface*             m_worker;
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

inline QJSValue QmlAct::returns() const{
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

inline const QJSValueList &QmlAct::argumentCallList() const{
    return m_argList;
}

inline QJSValue QmlAct::unwrap() const{
    return QJSValue();
}

} // namespace

#endif // LVQMLACT_H
