#ifndef LVQMLSTREAMOPERATOR_H
#define LVQMLSTREAMOPERATOR_H

#include <QObject>
#include "live/qmlstream.h"
#include "live/qmlstreamprovider.h"

namespace lv{

class QmlStreamOperator : public QObject, public QmlStreamProvider{

    Q_OBJECT

public:
    enum OperationType{
        StreamFunction = 1,
        ValueFunction,
        Act
    };

public:
    explicit QmlStreamOperator(ViewEngine* ve, OperationType ot, QmlStream* input, const QJSValue& options, const QJSValue& step, QObject *parent = nullptr);
    virtual ~QmlStreamOperator();

    lv::QmlStream* input() const;
    lv::QmlStream* output() const;

    static void streamFunctionHandler(QObject* that, const QJSValue& val);
    void applyStreamFunction(const QJSValue& val);

    static void valueFunctionHandler(QObject* that, const QJSValue& val);
    void applyValueFunction(const QJSValue& val);


    static void actHandler(QObject* that, const QJSValue& val);
    void applyAct(const QJSValue& val);

    // QmlStreamProvider interface
    void wait() override;
    void resume() override;

public slots:
    void ready();
    void push(const QJSValue& val);

private:
    ViewEngine*    m_engine;
    OperationType  m_operationType;
    lv::QmlStream* m_input;
    lv::QmlStream* m_output;
    QJSValue       m_step;
    QJSValueList   m_args;
    int            m_wait;
    bool           m_isRunning;
};

inline QmlStream *QmlStreamOperator::input() const{
    return m_input;
}

inline QmlStream *QmlStreamOperator::output() const{
    return m_output;
}

}// namespace

#endif // LVQMLSTREAMOPERATOR_H
