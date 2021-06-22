#ifndef LVQMLSTREAM_H
#define LVQMLSTREAM_H

#include <QObject>
#include <QJSValue>
#include <QQmlEngine>
#include <QQmlProperty>
#include "live/lvviewglobal.h"

namespace lv{

class ViewEngine;
class LV_VIEW_EXPORT QmlStream : public QObject{

    Q_OBJECT

public:
    class LV_VIEW_EXPORT Observer{

    public:
        enum Type{
            JsCallback,
            CppCallback,
            Property
        };

    public:
        virtual ~Observer();

        Type type(){ return m_type; }
        unsigned int id(){ return m_id; }

        virtual void push(ViewEngine* engine, const QJSValue& value) = 0;
        virtual void push(ViewEngine* engine, QObject* object) = 0;

    protected:
        Observer(unsigned int id, Type type) : m_id(id), m_type(type){}

    private:
        unsigned int m_id;
        Type         m_type;
    };

private:
    class JsCallbackObserver : public Observer{
    public:
        JsCallbackObserver(unsigned int id, QJSValue callback);
        void push(ViewEngine* engine, const QJSValue& value) override;
        void push(ViewEngine* engine, QObject* object) override;
    private:
        QJSValue m_callback;
    };

    class CppCallbackObserver : public Observer{
    public:
        typedef std::function<void(QObject*, const QJSValue& val)> CppCallback;

        CppCallbackObserver(unsigned int id, QObject* object, CppCallback cb);
        void push(ViewEngine* engine, const QJSValue& value) override;
        void push(ViewEngine* engine, QObject* object) override;
        QObject* object();

    private:
        QObject*    m_object;
        CppCallback m_callback;
    };

    class PropertyObserver : public Observer{
    public:
        PropertyObserver(unsigned int id, QJSValue callback);
        void push(ViewEngine* engine, const QJSValue& value) override;
        void push(ViewEngine* engine, QObject* object) override;
    private:
        QQmlProperty m_callback;
    };


public:
    explicit QmlStream(QObject *parent = nullptr);
    ~QmlStream();

    void push(QObject* object);
    void push(const QJSValue& value);

    Observer* forward(QObject* object, std::function<void(QObject*, const QJSValue& val)> fn);
    void unsubscribeObject(QObject* object);
    void unsubscribe(Observer* observer);

public slots:
    QJSValue forward(const QJSValue& callback);
    bool unsubscribe(const QJSValue& val);

private:
    void incrementIdCounter();
    void clearObservers();

    ViewEngine*  m_engine;
    unsigned int m_idCounter;
    std::list<QmlStream::Observer*>* m_observers;
};

}// namespace

#endif // LVQMLSTREAM_H
