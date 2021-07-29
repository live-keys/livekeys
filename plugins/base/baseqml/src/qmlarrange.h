#ifndef LVQMLARRANGE_H
#define LVQMLARRANGE_H

#include <QObject>
#include <QQmlParserStatus>
#include <QJSValue>

#include "live/viewengine.h"

namespace lv{

class QmlArrange : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QJSValue input  READ input  WRITE  setInput NOTIFY inputChanged)
    Q_PROPERTY(QJSValue map    READ map    WRITE  setMap   NOTIFY mapChanged)
    Q_PROPERTY(QJSValue result READ result NOTIFY resultChanged)

public:
    explicit QmlArrange(QObject *parent = nullptr);
    ~QmlArrange();

    QJSValue input() const;
    void setInput(QJSValue value);

    QJSValue map() const;
    void setMap(QJSValue map);

    QJSValue result() const;

signals:
    void inputChanged();
    void mapChanged();
    void resultChanged();

protected:
    void classBegin() override{}
    virtual void componentComplete() override;

private:
    void run();

    QJSValue    m_value;
    QJSValue    m_map;
    QJSValue    m_result;
    ViewEngine* m_engine;
};

inline QJSValue QmlArrange::input() const{
    return m_value;
}

inline QJSValue QmlArrange::map() const{
    return m_map;
}

inline QJSValue QmlArrange::result() const{
    return m_result;
}

inline void QmlArrange::setInput(QJSValue value){
    m_value = value;
    emit inputChanged();

    run();
}

inline void QmlArrange::setMap(QJSValue map){
    m_map = map;
    emit mapChanged();

    run();
}

}// namespace

#endif // LVQMLARRANGE_H
