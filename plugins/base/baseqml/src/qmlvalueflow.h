#ifndef LVQMLVALUEFLOW_H
#define LVQMLVALUEFLOW_H

#include <QObject>
#include <QQmlParserStatus>
#include <QJSValue>
#include <QQmlListProperty>

namespace lv{

class QmlValueFlow : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_PROPERTY(QJSValue value     READ value     WRITE setValue  NOTIFY valueChanged)
    Q_PROPERTY(QString  valueType READ valueType WRITE setValueType NOTIFY valueTypeChanged)
    Q_PROPERTY(QJSValue result    READ result    WRITE setResult NOTIFY resultChanged)
    Q_PROPERTY(QQmlListProperty<QObject> childObjects READ childObjects)
    Q_CLASSINFO("DefaultProperty", "childObjects")

public:
    explicit QmlValueFlow(QObject *parent = nullptr);
    ~QmlValueFlow() override;

    const QJSValue& value() const;
    void setValue(const QJSValue& value);

    const QJSValue &result() const;
    void setResult(const QJSValue &result);

    QQmlListProperty<QObject> childObjects();
    void appendChildObject(QObject* obj);
    int childObjectCount() const;
    QObject *childObject(int index) const;
    void clearChildObjects();

    const QString& valueType() const;
    void setValueType(const QString& valueType);

public slots:
    void exec();

signals:
    void valueChanged();
    void resultChanged();
    void valueTypeChanged();
    void complete();

protected:
    void classBegin() override{};
    void componentComplete() override;

private:
    static void appendChildObject(QQmlListProperty<QObject>*, QObject*);
    static int childObjectCount(QQmlListProperty<QObject>*);
    static QObject* childObject(QQmlListProperty<QObject>*, int);
    static void clearChildObjects(QQmlListProperty<QObject>*);

private:
    QJSValue m_value;
    QJSValue m_result;
    QString  m_valueType;
    bool     m_componentComplete;
    QList<QObject*> m_childObjects;

};

inline void QmlValueFlow::setValue(const QJSValue& value){
    m_value = value;
    emit valueChanged();
}

inline const QJSValue& QmlValueFlow::result() const{
    return m_result;
}

inline void QmlValueFlow::setResult(const QJSValue& result){
    m_result = result;
    emit resultChanged();
}

inline const QString &QmlValueFlow::valueType() const{
    return m_valueType;
}

inline const QJSValue& QmlValueFlow::value() const{
    return m_value;
}

}// namespace

#endif // QMLVALUEFLOW_H
