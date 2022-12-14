#ifndef LVQMLSTREAMOPERATIONS_H
#define LVQMLSTREAMOPERATIONS_H

#include <QObject>
#include <QJSValue>

#include "live/qmlstream.h"
#include "qmlstreamoperator.h"

namespace lv{

class QmlStreamOperations : public QObject{

    Q_OBJECT

public:
    explicit QmlStreamOperations(QObject *parent = nullptr);

public slots:
    lv::QmlStream* createProvider(const QJSValue& opt, const QJSValue& stepCb);
    lv::QmlStream* createOperator(QmlStream* input, const QJSValue& opt, const QJSValue& stepCb);
    lv::QmlStream* createValueOperator(QmlStream* input, const QJSValue& opt, const QJSValue& stepCb);
};

}// namespace

#endif // LVQMLSTREAMOPERATIONS_H
