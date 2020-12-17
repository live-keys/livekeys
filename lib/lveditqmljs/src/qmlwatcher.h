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

    const QString& referencedFile() const{ return m_referencedFile; }
    const QString& declaredId() const{ return m_declaredId; }

    bool isEnabled() const;

signals:
    void ready();

protected:
    void classBegin() override{}
    void componentComplete() override;

private:
    bool checkChildDeclarations();

    QString m_referencedFile;
    QString m_declaredId;
    bool    m_isEnabled;
    bool    m_componentComplete;
};

inline bool QmlWatcher::isEnabled() const{
    return m_isEnabled;
}

}// namespace

#endif // LVQMLWATCHER_H
