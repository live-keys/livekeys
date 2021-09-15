#ifndef LVQMLVALUEFLOWSINK_H
#define LVQMLVALUEFLOWSINK_H

#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>

namespace lv{

class QmlValueFlow;
class QmlStreamValueFlow;

class QmlValueFlowSink : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QJSValue value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit QmlValueFlowSink(QObject *parent = nullptr);
    ~QmlValueFlowSink();

    QJSValue value() const;
    void setValue(const QJSValue& value);

signals:
    void valueChanged();

protected:
    void classBegin() override{}
    void componentComplete() override;

private:
    void initializeParent();

private:
    bool                m_isComponentComplete;
    QmlValueFlow*       m_valueFlow;
    QmlStreamValueFlow* m_streamValueFlow;
};

}// namespace

#endif // LVQMLVALUEFLOWSINK_H
