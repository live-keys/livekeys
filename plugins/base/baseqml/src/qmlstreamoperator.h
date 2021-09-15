#ifndef LVQMLSTREAMOPERATOR_H
#define LVQMLSTREAMOPERATOR_H

#include <QObject>
#include "live/qmlstream.h"
#include "live/qmlstreamprovider.h"

namespace lv{

class QmlStreamOperator : public QObject, public QmlStreamProvider{

    Q_OBJECT
    Q_PROPERTY(lv::QmlStream* input  READ input  WRITE setInput NOTIFY inputChanged)
    Q_PROPERTY(lv::QmlStream* output READ output NOTIFY outputChanged)
    Q_PROPERTY(QJSValue next         READ next   WRITE setNext   NOTIFY nextChanged)

public:
    explicit QmlStreamOperator(QObject *parent = nullptr);
    virtual ~QmlStreamOperator();

    lv::QmlStream* input() const;
    void setInput(lv::QmlStream* stream);

    lv::QmlStream* output() const;

    QJSValue next() const;
    void setNext(QJSValue next);

    static void streamHandler(QObject* that, const QJSValue& val);
    void onStreamValue(const QJSValue& val);

    // QmlStreamProvider interface
    void wait() override;
    void resume() override;

public slots:
    void writeNext(QJSValue val);
    void ready();

signals:
    void inputChanged();
    void outputChanged();
    void nextChanged();

private:
    lv::QmlStream* m_input;
    lv::QmlStream* m_output;
    QJSValue       m_next;
    bool           m_isRunning;
    int            m_wait;
};

inline QmlStream *QmlStreamOperator::input() const{
    return m_input;
}

inline QmlStream *QmlStreamOperator::output() const{
    return m_output;
}

inline QJSValue QmlStreamOperator::next() const{
    return m_next;
}

}// namespace

#endif // LVQMLSTREAMOPERATOR_H
