#ifndef LVQMLSTREAM_H
#define LVQMLSTREAM_H

#include <QObject>
#include <QJSValue>
#include <QQmlEngine>
#include <QQmlProperty>
#include "live/lvviewglobal.h"

namespace lv{

class LV_VIEW_EXPORT QmlStream : public QObject{

    Q_OBJECT

public:
    explicit QmlStream(QObject *parent = nullptr);
    ~QmlStream();

    void push(QObject* object);
    void push(const QJSValue& value);

    void forward(QObject* object, std::function<void(QObject*, const QJSValue& val)> fn);

public slots:
    void forward(const QJSValue& callback);

private:
    QQmlEngine*  m_engine;
    QJSValue     m_callbackForward;
    QQmlProperty m_objectForward;

    QObject*     m_object;
    std::function<void(QObject*, const QJSValue& val)> m_functionForward;
};

}// namespace

#endif // LVQMLSTREAM_H
