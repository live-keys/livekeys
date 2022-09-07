#include "qmlpromise.h"
#include "live/visuallogqt.h"

namespace lv{

QmlPromise::QmlPromise(ViewEngine *engine, QJSValue fn, QObject *parent)
    : Shared(parent)
    , m_engine(engine)
    , m_state(QmlPromise::Pending)
{
    doResolve(fn);
}

QmlPromise::QmlPromise(ViewEngine *engine, QObject *parent)
    : Shared(parent)
    , m_engine(engine)
    , m_state(QmlPromise::Pending)
{
}


QmlPromise::~QmlPromise(){
}

QmlPromise *QmlPromise::resolve(ViewEngine *engine, QJSValue value, QObject *parent){
    QmlPromise* promise = new QmlPromise(engine, parent);
    promise->m_state = QmlPromise::FulFilled;
    promise->m_value = value;
    return promise;
}

QmlPromise *QmlPromise::reject(ViewEngine *engine, QJSValue value, QObject *parent){
    QmlPromise* promise = new QmlPromise(engine, parent);
    promise->m_state = QmlPromise::Rejected;
    promise->m_value = value;
    return promise;
}

QmlPromise *QmlPromise::fromValue(const QJSValue &v){
    if ( v.isQObject() ){
        return qobject_cast<QmlPromise*>(v.toQObject());
    }
    return nullptr;
}

void QmlPromise::then(const QmlPromise::Callback &fullfilled, const QmlPromise::Callback &rejected){
    if ( m_state == QmlPromise::Pending ){
        m_internalHandlers.push_back(std::make_pair(fullfilled, rejected));
    } else if ( m_state == QmlPromise::FulFilled && fullfilled ){
        fullfilled(m_value);
    } else if ( m_state == QmlPromise::Rejected && rejected ){
        rejected(m_value);
    }
}

void QmlPromise::clearInternalCallbacks(){
    m_internalHandlers.clear();
}

QmlPromise::Resolver *QmlPromise::Resolver::createWithPromise(ViewEngine *ve, QObject *parent){
    QmlPromise* promise = new QmlPromise(ve, parent);
    return new QmlPromise::Resolver(promise);
}

void QmlPromise::Resolver::resolve(QJSValue result){
    m_promise->__resolve(result);
}

void QmlPromise::Resolver::reject(QJSValue error){
    m_promise->__reject(error);
}

QmlPromise::Resolver::Resolver(QmlPromise *promise)
    : m_promise(promise)
{
    Shared::ref(m_promise);
}

QmlPromise::Resolver::~Resolver(){
    Shared::unref(m_promise);
}

void QmlPromise::fulfill(QJSValue result){
    m_state = QmlPromise::FulFilled;
    m_value = result;
    for ( auto it = m_handlers.begin(); it != m_handlers.end(); ++it ){
        std::pair<QJSValue, QJSValue>& p = *it;
        if ( p.first.isCallable() ){
            p.first.call(QJSValueList() << m_value);
        }
    }
    for ( auto it = m_internalHandlers.begin(); it != m_internalHandlers.end(); ++it ){
        if ( it->first )
            it->first(m_value);
    }
    m_handlers = QList<std::pair<QJSValue, QJSValue> >();
    m_internalHandlers = QList<std::pair<QmlPromise::Callback, QmlPromise::Callback> >();
}

void QmlPromise::__reject(QJSValue error){
    m_state = QmlPromise::Rejected;
    m_value = error;
    for ( auto it = m_handlers.begin(); it != m_handlers.end(); ++it ){
        std::pair<QJSValue, QJSValue>& p = *it;
        if ( p.second.isCallable() ){
            p.second.call(QJSValueList() << m_value);
        }
    }
    for ( auto it = m_internalHandlers.begin(); it != m_internalHandlers.end(); ++it ){
        if ( it->second )
            it->second(m_value);
    }
    m_handlers = QList<std::pair<QJSValue, QJSValue> >();
    m_internalHandlers = QList<std::pair<QmlPromise::Callback, QmlPromise::Callback> >();
}

QJSValue QmlPromise::getThen(QJSValue result){
    if ( result.isObject() ){
        QJSValue then = result.property("then");
        if ( then.isCallable() ){
            return then;
        }
    }
    return QJSValue();
}

void QmlPromise::__resolve(QJSValue result){
    QJSValue then = getThen(result);
    if ( then.isCallable() ){
        QJSValue thenWithResult = then.property("bind").callWithInstance(then, QJSValueList() << result);
        doResolve(thenWithResult);
    } else {
        fulfill(result);
    }
}

void QmlPromise::done(QJSValue onFulfilled, QJSValue onRejected){
    if ( m_state == QmlPromise::Pending ){
        m_handlers.append(std::make_pair(onFulfilled, onRejected));
    } else if ( m_state == QmlPromise::FulFilled && onFulfilled.isCallable() ){
        onFulfilled.call(QJSValueList() << m_value);
    } else if ( m_state == QmlPromise::Rejected && onRejected.isCallable() ){
        onRejected.call(QJSValueList() << m_value);
    }
}

QJSValue QmlPromise::then(QJSValue onFulfilled, QJSValue onRejected){

    QJSValue thenResolve = m_engine->engine()->evaluate("(function (self, onFulfilled, onRejected){\n"
    "    return function (resolve, reject) {\n"
    "        self.done( \n"
    "            function (result) {\n"
    "              if (typeof onFulfilled === 'function') {\n"
    "                try {\n"
    "                  return resolve(onFulfilled(result))\n"
    "                } catch (ex) {\n"
    "                  return reject(ex)\n"
    "                }\n"
    "              } else {\n"
    "                return resolve(result)\n"
    "              }\n"
    "            },\n"
    "            function (error) {\n"
    "              if (typeof onRejected === 'function') {\n"
    "                try {\n"
    "                  return resolve(onRejected(error));\n"
    "                } catch (ex) {\n"
    "                  return reject(ex);\n"
    "                }\n"
    "              } else {\n"
    "                return reject(error);\n"
    "              }\n"
    "        });\n"
    "    }\n"
    "})\n");

    QJSValue promiseFn = thenResolve.call(QJSValueList() << m_engine->engine()->newQObject(this) << onFulfilled << onRejected);
    QmlPromise* promise = new QmlPromise(m_engine, promiseFn);
    return m_engine->engine()->newQObject(promise);
}

QJSValue QmlPromise::capture(QJSValue onRejected){
    return then(QJSValue(), onRejected);
}

void QmlPromise::doResolve(QJSValue fn){
    QJSValue resolveTemplate = m_engine->engine()->evaluate(
        "(function(promise){"
        "    var done = false;\n"
        "    var onFulfilled = function(value){\n"
        "        if (done) return\n"
        "        done = true\n"
        "        promise.__resolve(value);\n"
        "    }\n"
        "    var onRejected = function(value){\n"
        "        if ( done ) return\n"
        "        done = true\n"
        "        promise.__reject(value)\n"
        "    }\n"
        "    return { onFulfilled: onFulfilled, onRejected: onRejected }\n"
        "})"
    );

    QJSValue resolveObject = resolveTemplate.call(QJSValueList() << m_engine->engine()->newQObject(this));

    QJSValue result = fn.call(QJSValueList() << resolveObject.property("onFulfilled") << resolveObject.property("onRejected"));
    if ( result.isError() ){
        resolveObject.property("onRejected").call(QJSValueList() << result);
    }
}

}// namespace
