#ifndef LVQMLEACHVALUEAPPLY_H
#define LVQMLEACHVALUEAPPLY_H

#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>

#include "live/viewengine.h"

namespace lv{

class QmlEachValueApplyPrivate;
class QmlEachValueApply : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QJSValue input      READ input  WRITE  setInput  NOTIFY inputChanged)
    Q_PROPERTY(QJSValue result     READ result NOTIFY resultChanged)
    Q_PROPERTY(QQmlComponent* flow READ flow   WRITE  setFlow   NOTIFY flowChanged)

public:
    explicit QmlEachValueApply(QObject *parent = nullptr);
    ~QmlEachValueApply() override;

    const QJSValue& input() const;
    void setInput(const QJSValue& input);

    QQmlComponent* flow() const;
    void setFlow(QQmlComponent* flow);

    const QJSValue &result() const;

public slots:
    void exec();
    void __flowResultReady();

signals:
    void inputChanged();
    void resultChanged();
    void flowChanged();

protected:
    void classBegin() override{}
    virtual void componentComplete() override;

private:
    Q_DECLARE_PRIVATE(QmlEachValueApply)

    QScopedPointer<QmlEachValueApplyPrivate> d_ptr;

    bool           m_isRunning;
    bool           m_isComponentComplete;
    ViewEngine*    m_engine;
    QQmlComponent* m_flow;
    QJSValue       m_input;
    QJSValue       m_result;
    QJSValue       m_newResult;
};

inline const QJSValue &QmlEachValueApply::input() const{
    return m_input;
}

inline const QJSValue& QmlEachValueApply::result() const{
    return m_result;
}

inline QQmlComponent *QmlEachValueApply::flow() const{
    return m_flow;
}

}// namespace

#endif // LVQMLEACHVALUEAPPLY_H
