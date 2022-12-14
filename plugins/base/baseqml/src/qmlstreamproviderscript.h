#ifndef LVQMLSTREAMPROVIDERSCRIPT_H
#define LVQMLSTREAMPROVIDERSCRIPT_H

#include <QObject>
#include <QJSValue>

#include "live/qmlwritablestream.h"
#include "live/qmlstreamprovider.h"

namespace lv{

class QmlStreamProviderScript : public QObject, public QmlStreamProvider{

    Q_OBJECT

public:
    explicit QmlStreamProviderScript(ViewEngine* ve, const QJSValue& options, const QJSValue& step, QObject *parent = nullptr);
    ~QmlStreamProviderScript();

    void wait() override;
    void resume() override;
    void next();

    QmlStream* stream() const;

public slots:
    void push(const QJSValue& val);
    void close();

signals:

private:
    ViewEngine*   m_engine;
    QmlStream*    m_stream;
    QJSValueList  m_args;
    QJSValue      m_step;
    int           m_wait;
};

inline QmlStream *QmlStreamProviderScript::stream() const{
    return m_stream;
}

}// namespace

#endif // QMLSTREAMPROVIDERSCRIPT_H
