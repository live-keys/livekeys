#ifndef LVQMLSTREAMVALUEFLOW_H
#define LVQMLSTREAMVALUEFLOW_H

#include <QObject>
#include <QJSValue>
#include <QQmlListProperty>
#include "live/qmlstream.h"
#include "live/qmlstreamprovider.h"

namespace lv{

class QmlStreamValueFlow : public QObject, public QmlStreamProvider, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QJSValue value       READ value        NOTIFY valueChanged)
    Q_PROPERTY(QString valueType    READ valueType    WRITE setValueType NOTIFY valueTypeChanged)
    Q_PROPERTY(QJSValue result      READ result       WRITE setResult    NOTIFY resultChanged)
    Q_PROPERTY(lv::QmlStream* input READ input        WRITE setInput     NOTIFY inputChanged)
    Q_PROPERTY(lv::QmlStream* output READ output      NOTIFY outputChanged)
    Q_PROPERTY(QQmlListProperty<QObject> childObjects READ childObjects)
    Q_CLASSINFO("DefaultProperty", "childObjects")

public:
    explicit QmlStreamValueFlow(QObject *parent = nullptr);
    ~QmlStreamValueFlow() override;

    const QJSValue& value() const;

    const QString& valueType() const;
    void setValueType(const QString& valueType);

    const QJSValue &result() const;
    void setResult(const QJSValue &result);

    lv::QmlStream* input() const;
    void setInput(lv::QmlStream* stream);

    lv::QmlStream* output() const;

    static void streamHandler(QObject* that, const QJSValue& val);
    void onStreamValue(const QJSValue& val);

    QQmlListProperty<QObject> childObjects();
    void appendChildObject(QObject* obj);
    int childObjectCount() const;
    QObject *childObject(int index) const;
    void clearChildObjects();

    // QmlStreamProvider interface
    void wait() override;
    void resume() override;

    // QQmlParserStatus interface
    void classBegin() override{}
    void componentComplete() override;

signals:
    void valueChanged();
    void valueTypeChanged();
    void resultChanged();
    void outputChanged();
    void inputChanged();

private:
    static void appendChildObject(QQmlListProperty<QObject>*, QObject*);
    static int childObjectCount(QQmlListProperty<QObject>*);
    static QObject* childObject(QQmlListProperty<QObject>*, int);
    static void clearChildObjects(QQmlListProperty<QObject>*);

private:
    bool            m_isComponentComplete;
    QJSValue        m_value;
    QJSValue*       m_next;
    QString         m_valueType;
    QJSValue        m_result;
    QList<QObject*> m_childObjects;
    lv::QmlStream*  m_input;
    lv::QmlStream*  m_output;
    bool            m_isRunning;
    bool            m_inStreamValue;
    int             m_wait;
};

inline const QJSValue& QmlStreamValueFlow::result() const{
    return m_result;
}

inline QmlStream *QmlStreamValueFlow::input() const{
    return m_input;
}

inline QmlStream *QmlStreamValueFlow::output() const{
    return m_output;
}

inline const QJSValue& QmlStreamValueFlow::value() const{
    return m_value;
}

inline const QString &QmlStreamValueFlow::valueType() const{
    return m_valueType;
}

}// namespace

#endif // LVQMLSTREAMVALUEFLOW_H
