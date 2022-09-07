#ifndef QMLPROMISE_H
#define QMLPROMISE_H

#include <QObject>
#include <QJSValue>
#include "live/viewengine.h"
#include "live/shared.h"

namespace lv{

class LV_VIEW_EXPORT QmlPromise : public Shared{

    Q_OBJECT

public:
    enum State{
        Pending = 0,
        FulFilled = 1,
        Rejected = 2
    };

    typedef std::function<void(QJSValue)> Callback;

    class LV_VIEW_EXPORT Resolver{
    public:
        static Resolver* createWithPromise(ViewEngine* ve, QObject* parent = nullptr);

        void resolve(QJSValue result);
        void reject(QJSValue result);
        QmlPromise* promise(){ return m_promise; }
    private:
        Resolver(QmlPromise* promise);
        ~Resolver();

        QmlPromise* m_promise;
    };

public:
    explicit QmlPromise(ViewEngine* engine, QJSValue fn, QObject *parent = nullptr);
    virtual ~QmlPromise();

    static QmlPromise* resolve(ViewEngine* engine, QJSValue value, QObject* parent = nullptr);
    static QmlPromise* reject(ViewEngine* engine, QJSValue value, QObject* parent = nullptr);

    static QmlPromise* fromValue(const QJSValue& v);

    void then(const Callback& fullfilled, const Callback& rejected = nullptr);
    void clearInternalCallbacks();

public slots:
    void __reject(QJSValue error);
    void __resolve(QJSValue result);
    void done(QJSValue onFulfilled, QJSValue onRejected);
    QJSValue then(QJSValue onFulfilled, QJSValue onRejected = QJSValue());
    QJSValue capture(QJSValue onRejected);

private:
    QmlPromise(ViewEngine* engine, QObject* parent = nullptr);

    void fulfill(QJSValue result);
    QJSValue getThen(QJSValue result);
    void doResolve(QJSValue fn);

    ViewEngine*    m_engine;
    State          m_state;
    QJSValue       m_value;
    QList<std::pair<Callback, Callback> > m_internalHandlers;
    QList<std::pair<QJSValue, QJSValue> > m_handlers;
};

}// namespace

#endif // QMLPROMISE_H
