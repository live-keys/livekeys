#ifndef LVQMLCOLLECTOR_H
#define LVQMLCOLLECTOR_H

#include <QObject>
#include <QQmlParserStatus>
#include <list>

#include "live/viewengine.h"

namespace lv{

class QmlPropertyWatcher;
class QmlCollector : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QObject* resetter READ resetter WRITE setResetter NOTIFY resetterChanged)
    Q_PROPERTY(QJSValue result   READ result   NOTIFY resultChanged)

public:
    explicit QmlCollector(QObject *parent = nullptr);
    ~QmlCollector();

    QJSValue result() const;

    static void propertyChange(const QmlPropertyWatcher& watcher);

    QObject* resetter() const;
    void setResetter(QObject* resetter);

public slots:
    void release();
    void reset();

signals:
    void resultChanged();
    void resetterChanged();
    void complete();

protected:
    void classBegin() override{}
    virtual void componentComplete() override;

private:
    void collectProperty(const QString& name, const QVariant &value);

    ViewEngine*                     m_engine;
    std::list<QmlPropertyWatcher*>* m_properties;
    std::list<QString>*             m_propertiesToCollect;
    QJSValue                        m_collectingResult;
    QJSValue                        m_result;
    QObject*                        m_resetter;
};

inline QObject *QmlCollector::resetter() const{
    return m_resetter;
}

inline QJSValue QmlCollector::result() const{
    return m_result;
}

}// namespace

#endif // LVQMLCOLLECTOR_H
