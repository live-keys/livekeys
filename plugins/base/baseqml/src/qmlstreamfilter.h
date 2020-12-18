#ifndef LVQMLSTREAMFILTER_H
#define LVQMLSTREAMFILTER_H

#include <QObject>
#include <QJSValue>
#include "live/lvbaseqmlglobal.h"
#include "live/qmlstream.h"
#include "live/qmlwritablestream.h"

namespace lv{

class WorkerThread;

class LV_BASEQML_EXPORT QmlStreamFilter : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::QmlStream* pull           READ pull    WRITE setPull NOTIFY pullChanged)
    Q_PROPERTY(QJSValue current              READ current NOTIFY currentChanged)
    Q_PROPERTY(lv::QmlWritableStream* result READ result  CONSTANT)
    Q_PROPERTY(QQmlListProperty<QObject> childObjects READ childObjects)
    Q_CLASSINFO("DefaultProperty", "childObjects")

public:
    QmlStreamFilter(QObject* parent = nullptr);
    ~QmlStreamFilter();

    lv::QmlStream* pull() const;
    lv::QmlWritableStream* result() const;
    QJSValue current() const;

    void setPull(lv::QmlStream* pull);

    static void streamHandler(QObject* that, const QJSValue& val);


    QQmlListProperty<QObject> childObjects();
    void appendChildObject(QObject* obj);
    int childObjectCount() const;
    QObject *childObject(int) const;
    void clearChildObjects();

signals:
    void pullChanged();
    void currentChanged();

private:
    static void appendChildObject(QQmlListProperty<QObject>*, QObject*);
    static int childObjectCount(QQmlListProperty<QObject>*);
    static QObject* childObject(QQmlListProperty<QObject>*, int);
    static void clearChildObjects(QQmlListProperty<QObject>*);

    void setCurrent(const QJSValue& val);

    lv::QmlStream*         m_pull;
    lv::QmlWritableStream* m_result;
    QJSValue               m_current;
    QVector<QObject*>      m_childObjects;
};

inline QmlStream *QmlStreamFilter::pull() const{
    return m_pull;
}

inline QmlWritableStream *QmlStreamFilter::result() const{
    return m_result;
}

inline QJSValue QmlStreamFilter::current() const{
    return m_current;
}

}// namespace

#endif // LVQMLSTREAMFILTER_H
