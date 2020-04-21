#ifndef LVQMLWATCHER_H
#define LVQMLWATCHER_H

#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>

namespace lv{

class QmlWatcherBackground;
class QmlWatcher : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_PROPERTY(QObject* scanner READ scanner CONSTANT)
    Q_PROPERTY(QJSValue position READ position WRITE setPosition NOTIFY positionChanged)
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit QmlWatcher(QObject *parent = nullptr);
    ~QmlWatcher() override;

    QObject* scanner();
    const QJSValue& position() const;

    const QString& fileReference() const;

public slots:
    void setPosition(QJSValue position);
    void __workerResultReady();

protected:
    void classBegin() override{}
    void componentComplete() override;

signals:
    void ready();
    void positionChanged();

private:
    QObject* m_scanner;
    QString  m_filePath;
    int      m_fileLine;
    QJSValue m_position;

    QmlWatcherBackground* m_worker;
    bool     m_componentComplete;
};

}// namespace

#endif // LVQMLWATCHER_H
