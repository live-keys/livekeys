#ifndef LVQMLWATCHER_H
#define LVQMLWATCHER_H

#include "live/lveditqmljsglobal.h"

#include <QObject>
#include <QJSValue>
#include <QQmlParserStatus>

namespace lv{

class ViewEngine;
class HookContainer;

class LV_EDITQMLJS_EXPORT QmlWatcher : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit QmlWatcher(QObject *parent = nullptr);
    ~QmlWatcher() override;

    const QString& referencedFile() const{ return m_referencedFile; }
    const QString& declaredId() const{ return m_declaredId; }

    bool isEnabled() const;

    void initialize(ViewEngine* engine, HookContainer* hooks, const QString& referencedFile, const QString& declaredId);

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
