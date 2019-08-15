#ifndef LVRUNNABLE_H
#define LVRUNNABLE_H

#include "live/lveditorglobal.h"
#include "live/projectdocument.h"

#include <QObject>
#include <QQmlComponent>

namespace lv{

class ViewEngine;
class LV_EDITOR_EXPORT Runnable : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString path READ path CONSTANT)

public:
    enum Type{
        File,
        Component
    };

public:
    Runnable(ViewEngine* engine, const QString &path, const QString& name = "", QObject* parent = nullptr);
    Runnable(ViewEngine* engine, QQmlComponent* component, const QString& name = "", QObject* parent = nullptr);
    ~Runnable();

    const QString& name() const;
    void setName(const QString& name);

    void setRunSpace(QObject* runspace);
    QObject* appRoot();

    void run();

    const QString& path() const;

public slots:
    void engineObjectAcquired(const QUrl& file, QObject* ref);
    void engineObjectReady(QObject* object, const QUrl& file, QObject* ref);
    void engineObjectCreationError(QJSValue errors, const QUrl&, QObject* reference);

    QObject* runSpace();

signals:
    void nameChanged(QString name);
    void runError(QJSValue errors);

private:
    void emptyRunSpace();

    QString               m_name;
    QString               m_path;
    QQmlComponent*        m_component;
    QObject*              m_runSpace;
    ViewEngine*           m_engine;
    QObject*              m_appRoot;
    Type                  m_type;
};

inline const QString &Runnable::name() const{
    return m_name;
}

inline void Runnable::setName(const QString &name){
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged(m_name);
}

inline void Runnable::setRunSpace(QObject *runspace){
    m_runSpace = runspace;
}

inline QObject *Runnable::runSpace(){
    return m_runSpace;
}

inline QObject *Runnable::appRoot(){
    return m_appRoot;
}

inline const QString &Runnable::path() const{
    return m_path;
}

}// namespace

#endif // LVRUNNABLE_H
