#ifndef LVQMLWATCHER_H
#define LVQMLWATCHER_H

#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>

namespace lv{

class QmlWatcherBackground;
class QmlWatcher : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit QmlWatcher(QObject *parent = nullptr);
    ~QmlWatcher() override;

    const QString& fileReference() const{ return m_file; }
    const QString& declaredId() const{ return m_id; }

signals:
    void ready();

protected:
    void classBegin() override{}
    void componentComplete() override;

private:
    QString m_file;
    QString m_id;
    bool    m_componentComplete;
};

}// namespace

#endif // LVQMLWATCHER_H
