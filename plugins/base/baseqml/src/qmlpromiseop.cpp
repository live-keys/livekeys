#include "qmlpromiseop.h"

namespace lv{

// TODO: Add PromiseOp.all
//PromiseOp.all = function promiseAllIterative(values) {
//    return new Promise((resolve, reject) => {
//       let results = [];
//       let completed = 0;

//       values.forEach((value, index) => {
//            Promise.resolve(value).then(result => {
//                results[index] = result;
//                completed += 1;

//                if (completed == values.length) {
//                    resolve(results);
//                }
//            }).catch(err => reject(err));
//       });
//    });
//}

QmlPromiseOp::QmlPromiseOp(QQmlEngine *engine, QObject *parent)
    : QObject(parent)
    , m_engine(nullptr)
{
    m_engine = qobject_cast<ViewEngine*>(engine->property("viewEngine").value<QObject*>());
}

QmlPromise *QmlPromiseOp::create(QJSValue fn){
    return new QmlPromise(m_engine, fn, nullptr);
}

QmlPromise *QmlPromiseOp::resolve(QJSValue val){
    return QmlPromise::resolve(m_engine, val);
}

QmlPromise *QmlPromiseOp::reject(QJSValue val){
    return QmlPromise::reject(m_engine, val);
}

}// namespace
