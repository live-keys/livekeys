#ifndef LVQMLVALUEFLOWINPUT_H
#define LVQMLVALUEFLOWINPUT_H

#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>

namespace lv{

class QmlValueFlow;
class QmlStreamValueFlow;

class QmlValueFlowInput : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QJSValue value READ value NOTIFY valueChanged)

public:
    explicit QmlValueFlowInput(QObject *parent = nullptr);
    ~QmlValueFlowInput();

    QJSValue value();

signals:
    void valueChanged();

protected:
    void classBegin() override;
    void componentComplete() override;

private:
    void initializeVars();
    void initializeParent();

private:
    bool                m_isComponentComplete;
    QmlValueFlow*       m_valueFlow;
    QmlStreamValueFlow* m_streamValueFlow;
};

}// namespace

#endif // LVQMLVALUEFLOWINPUT_H
