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
    enum Type{
        Imports,
        Property,
        ListIndex,
        Method,
        Object
    };

    typedef QSharedPointer<QmlBindingChannel>       Ptr;
    typedef QSharedPointer<const QmlBindingChannel> ConstPtr;

public:
    static QmlBindingChannel::Ptr create(
        QmlBindingPath::Ptr bindingPath,
        Runnable* runnable,
        QObject* object = nullptr);
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
    static QmlBindingChannel::Ptr createForImports(
        QmlBindingPath::Ptr bindingPath,
        Runnable* runnable);

    QmlBindingChannel(QmlBindingPath::Ptr bindingPath, Runnable* runnable, QObject *parent = nullptr);
    virtual ~QmlBindingChannel();

    bool hasConnection() const;
    void updateConnection(const QQmlProperty& property, int listIndex = -1);
    void updateConnection(const QQmlProperty& property, const QMetaMethod &method);
    void updateConnection(QObject* object);
    void updateConnection(int listIndex);
    void clearConnection();

    Type type() const;

    const QmlBindingPath::ConstPtr& bindingPath() const{ return m_bindingPath; }
    Runnable* runnable();
    int listIndex() const{ return m_listIndex; }
    QQmlProperty& property(){ return m_property; }
    bool isEnabled() const{ return m_enabled; }
    bool canModify() const;
    void rebuild();
    QObject* object() const;

    bool isBuilder() const;
    void setIsBuilder(bool isBuilder);

    const QMetaMethod& method() const;

    QmlBindingPath::Ptr expressionPath();

    void setEnabled(bool enable);

private:
    QmlBindingPath::Ptr m_bindingPath;
    Runnable*           m_runnable;
    QObject*            m_object;
    QQmlProperty        m_property;
    int                 m_listIndex;
    QMetaMethod         m_method;
    bool                m_enabled;
    bool                m_isBuilder;
    Type                m_type;
};

inline bool QmlBindingChannel::hasConnection() const{
    return m_property.isValid() || m_object;
}

inline Runnable *QmlBindingChannel::runnable(){
    return m_runnable;
}

inline bool QmlBindingChannel::canModify() const{
    return m_enabled && hasConnection();
}

inline bool QmlBindingChannel::isBuilder() const{
    return m_isBuilder;
}

inline void QmlBindingChannel::setIsBuilder(bool isBuilder){
    m_isBuilder = isBuilder;
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

inline void QmlBindingChannel::updateConnection(QObject *object){
    m_object = object;
}

inline void QmlBindingChannel::clearConnection(){
    m_property  = QQmlProperty();
    m_listIndex = -1;
    m_method = QMetaMethod();
    m_object = nullptr;
}


inline QmlBindingChannel::Type QmlBindingChannel::type() const{
    if ( m_listIndex > -1 ){
        return QmlBindingChannel::ListIndex;
    } else if ( m_method.isValid() ){
        return QmlBindingChannel::Method;
    } else if ( m_property.object() ){
        return QmlBindingChannel::Property;
    }
    return QmlBindingChannel::Object;
}

}// namespace

#endif // LVQMLBINDINGCHANNEL_H
