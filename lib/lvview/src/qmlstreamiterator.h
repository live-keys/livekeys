#ifndef LVQMLSTREAMITERATOR_H
#define LVQMLSTREAMITERATOR_H

#include <QObject>
#include "qmlstream.h"

namespace lv{

class QmlStreamIterator : public QObject{

    Q_OBJECT
    Q_PROPERTY(lv::QmlStream* stream READ stream  WRITE setStream NOTIFY streamChanged)
    Q_PROPERTY(QJSValue current      READ current NOTIFY currentChanged)

public:
    explicit QmlStreamIterator(QObject *parent = nullptr);

    lv::QmlStream* stream() const;
    void setStream(lv::QmlStream* stream);

    QJSValue current() const;

    static void streamHandler(QObject* that, const QJSValue& val);

signals:
    void streamChanged();
    void currentChanged();

private:
    lv::QmlStream* m_stream;
    QJSValue       m_current;
};

inline QmlStream *QmlStreamIterator::stream() const{
    return m_stream;
}

inline void QmlStreamIterator::setStream(QmlStream *stream){
    if (m_stream == stream)
        return;

    if( m_stream )
        m_stream->forward(nullptr, nullptr);

    m_stream = stream;
    m_stream->forward(this, &QmlStreamIterator::streamHandler);

    emit streamChanged();
}

inline QJSValue QmlStreamIterator::current() const{
    return m_current;
}

}// namespace

#endif // LVQMLSTREAMITERATOR_H
