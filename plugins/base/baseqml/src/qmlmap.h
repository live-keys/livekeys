#ifndef LVQMLMAP_H
#define LVQMLMAP_H

#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>

#include "live/viewengine.h"
#include "qmlvalueflowgraph.h"

namespace lv{

class QmlMap : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QJSValue input           READ input  WRITE  setInput  NOTIFY inputChanged)
    Q_PROPERTY(QJSValue result          READ result NOTIFY resultChanged)
    Q_PROPERTY(lv::QmlValueFlowGraph* f READ f      WRITE  setF      NOTIFY flowChanged)

public:
    explicit QmlMap(QObject *parent = nullptr);
    ~QmlMap() override;

    const QJSValue& input() const;
    void setInput(const QJSValue& input);

    QmlValueFlowGraph* f() const;
    void setF(QmlValueFlowGraph* flow);

    const QJSValue &result() const;



public slots:
    void exec();
    QJSValue evaluate(const QJSValue& input);

signals:
    void inputChanged();
    void resultChanged();
    void flowChanged();

protected:
    void classBegin() override{}
    virtual void componentComplete() override;

private:
    bool               m_isRunning;
    bool               m_isComponentComplete;
    ViewEngine*        m_engine;
    QmlValueFlowGraph* m_flow;
    QJSValue           m_input;
    QJSValue           m_result;
    QJSValue           m_newResult;
};

inline const QJSValue &QmlMap::input() const{
    return m_input;
}

inline const QJSValue& QmlMap::result() const{
    return m_result;
}

inline QmlValueFlowGraph *QmlMap::f() const{
    return m_flow;
}

}// namespace

#endif // LVQMLMAP_H
