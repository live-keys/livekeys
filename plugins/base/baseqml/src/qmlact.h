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
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QJSValue run                   READ run     WRITE setRun     NOTIFY runChanged)
    Q_PROPERTY(QJSValue args                  READ args    WRITE setArgs    NOTIFY argsChanged)
    Q_PROPERTY(QString returns                READ returns WRITE setReturns NOTIFY returnsChanged)
    Q_PROPERTY(QJSValue result                READ result  NOTIFY resultChanged)
    Q_PROPERTY(lv::QmlWorkerInterface* worker READ worker  WRITE setWorker NOTIFY workerChanged)

public:
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
    void setArgs(QJSValue args);
    const QJSValue& args() const;
    const QJSValue& run() const;

    void setResult(const QJSValue& result);
    void setResult(const QVariant& result);

    QString returns() const;
    void setReturns(QString returns);

    lv::QmlWorkerInterface* worker() const;

    bool event(QEvent *event) override;

public slots:
    void exec();
    void setWorker(lv::QmlWorkerInterface* worker);

signals:
    void complete();
    void runChanged();
    void resultChanged();
    void argsChanged();
    void workerChanged();
    void returnsChanged();

protected:
    void classBegin() override{}
    virtual void componentComplete() override;

private:
    void extractSource(ViewEngine* engine);

    bool     m_isComponentComplete;
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

} // namespace

#endif // LVQMLACT_H
