#ifndef LVQMLBINDINGCHANNEL_H
#define LVQMLBINDINGCHANNEL_H

#include <QObject>
#include <QQmlProperty>

#include "live/projectdocument.h"
#include "live/visuallog.h"
#include "live/runnable.h"
#include "qmlbindingpath.h"

namespace lv{

class QmlBindingSpan;

/// \private
class QmlBindingChannel : public QObject{

    Q_OBJECT

public:
    typedef QSharedPointer<QmlBindingChannel>       Ptr;
    typedef QSharedPointer<const QmlBindingChannel> ConstPtr;

public:
    static QmlBindingChannel::Ptr create(QmlBindingPath::Ptr bindingPath, Runnable* runnable);
    static QmlBindingChannel::Ptr create(
        QmlBindingPath::Ptr bindingPath,
        Runnable* runnable,
        const QQmlProperty& property,
        int listIndex = -1
    );
    static QmlBindingChannel::Ptr create(
        QmlBindingPath::Ptr bindingPath,
        Runnable* runnable,
        const QQmlProperty& property,
        const QMetaMethod& method
    );

    QmlBindingChannel(QmlBindingPath::Ptr bindingPath, Runnable* runnable, QObject *parent = nullptr);
    virtual ~QmlBindingChannel();

    bool hasConnection() const;
    void updateConnection(const QQmlProperty& property, int listIndex = -1);
    void updateConnection(const QQmlProperty& property, const QMetaMethod &method);
    void clearConnection();

    const QmlBindingPath::ConstPtr& bindingPath() const{ return m_bindingPath; }
    Runnable* runnable();
    int listIndex() const{ return m_listIndex; }
    QQmlProperty& property(){ return m_property; }
    bool isEnabled() const{ return m_enabled; }
    bool canModify() const;

    const QMetaMethod& method() const;

    QmlBindingPath::Ptr expressionPath();

    void setEnabled(bool enable);

    void commit(const QVariant& value);

public slots:
    void __runableReady();

signals:
    void runnableObjectReady();

private:
    QmlBindingPath::Ptr m_bindingPath;
    Runnable*           m_runnable;
    QQmlProperty        m_property;
    int                 m_listIndex;
    QMetaMethod         m_method;
    bool                m_enabled;
};

inline bool QmlBindingChannel::hasConnection() const{
    return m_property.isValid();
}

inline Runnable *QmlBindingChannel::runnable(){
    return m_runnable;
}

inline bool QmlBindingChannel::canModify() const{
    return m_enabled && hasConnection();
}

inline const QMetaMethod &QmlBindingChannel::method() const{
    return m_method;
}

inline void QmlBindingChannel::setEnabled(bool enable){
    m_enabled = enable;
}

inline void QmlBindingChannel::updateConnection(const QQmlProperty &property, int listIndex){
    m_property = property;
    m_listIndex = listIndex;
}

inline void QmlBindingChannel::updateConnection(const QQmlProperty &property, const QMetaMethod &method){
    m_property = property;
    m_method = method;
}

inline void QmlBindingChannel::clearConnection(){
    m_property  = QQmlProperty();
    m_listIndex = -1;
    m_method = QMetaMethod();
}

}// namespace

#endif // LVQMLBINDINGCHANNEL_H
