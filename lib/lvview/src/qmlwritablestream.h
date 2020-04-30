#ifndef LVQMLWRITABLESTREAM_H
#define LVQMLWRITABLESTREAM_H

#include <QObject>
#include "qmlstream.h"

namespace lv{

class LV_VIEW_EXPORT QmlWritableStream : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::QmlStream* stream READ stream CONSTANT)

public:
    explicit QmlWritableStream(QObject *parent = nullptr);
    ~QmlWritableStream();

    QmlStream* stream() const;

public slots:
    void push(const QJSValue& val);

private:
    lv::QmlStream* m_stream;
};

inline QmlStream *QmlWritableStream::stream() const{
    return m_stream;
}

inline void QmlWritableStream::push(const QJSValue &val){
    m_stream->push(val);
}

}// namespace

#endif // LVQMLWRITABLESTREAM_H
