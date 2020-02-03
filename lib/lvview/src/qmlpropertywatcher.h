#ifndef LVQMLPROPERTYWATCHER_H
#define LVQMLPROPERTYWATCHER_H

#include <QObject>
#include <QQmlProperty>
#include <functional>
#include "live/lvviewglobal.h"

namespace lv{

class LV_VIEW_EXPORT QmlPropertyWatcher : public QObject{

    Q_OBJECT

public:
    QmlPropertyWatcher(QObject* object, const QString& propertyName, QObject *parent = nullptr);
    QmlPropertyWatcher(const QQmlProperty& p, QObject* parent = nullptr);
    ~QmlPropertyWatcher();

    QVariant read() const;

    void onChange(std::function<void(const QmlPropertyWatcher&)> handler);

    QObject* object() const;
    QString name() const;
    bool isValid() const;

public slots:
    void propertyChanged() const;

private:
    QQmlProperty m_property;

    std::function<void(const QmlPropertyWatcher&)> m_propertyChange;

};

inline QVariant QmlPropertyWatcher::read() const{
    return m_property.read();
}

inline void QmlPropertyWatcher::onChange(std::function<void (const QmlPropertyWatcher &)> handler){
    m_propertyChange = handler;
}

inline QObject *QmlPropertyWatcher::object() const{
    return m_property.object();
}

inline QString QmlPropertyWatcher::name() const{
    return m_property.name();
}

inline bool QmlPropertyWatcher::isValid() const{
    return m_property.isValid();
}

}// namespace

#endif // LVQMLPROPERTYWATCHER_H
