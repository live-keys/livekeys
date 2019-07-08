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
    explicit QmlStream(QObject *parent = 0);
    ~QmlStream();

    void push(QObject* object);

public slots:
    void forward(const QJSValue& callback);

private:
    QQmlEngine*  m_engine;
    QJSValue     m_callbackForward;
    QQmlProperty m_objectForward;
};

}// namespace

#endif // QMLSTREAM_H
