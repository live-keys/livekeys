#ifndef LVRUNNABLE_H
#define LVRUNNABLE_H

#include "live/lveditorglobal.h"
#include "live/projectdocument.h"

#include <QObject>
#include <QQmlComponent>

namespace lv{

class ViewEngine;
class RunnableContainer;

namespace el{
    class Engine;
    class Element;
}

class LV_EDITOR_EXPORT Runnable : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name   READ name       WRITE setName       NOTIFY nameChanged)
    Q_PROPERTY(int runTrigger READ runTrigger WRITE setRunTrigger NOTIFY runTriggerChanged)
    Q_PROPERTY(QString path   READ path CONSTANT)

public:
    enum Type{
        QmlFile,
        QmlComponent,
        LvFile
    };

public:
    Runnable(
        ViewEngine* viwEngine,
        el::Engine* engine,
        const QString &path,
        RunnableContainer* parent,
        const QString& name = "",
        const QSet<QString>& activations = QSet<QString>());

    Runnable(
        ViewEngine* engine,
        QQmlComponent* component,
        RunnableContainer* parent,
        const QString& name = "");

    ~Runnable();

    const QString& name() const;
    void setName(const QString& name);

    QObject* viewRoot();
    el::Element* elementRoot();

    const QSet<QString>& activations() const;

    const QString& path() const;

    int runTrigger() const;

    Type type() const;

public slots:
    void engineObjectAcquired(const QUrl& file, QObject* ref);
    void engineObjectReady(QObject* object, const QUrl& file, QObject* ref, QQmlContext* context);
    void engineObjectCreationError(QJSValue errors, const QUrl&, QObject* reference, QQmlContext* context);

    void run();

    void setRunSpace(QObject* runspace);
    QObject* runSpace();

    void _activationContentsChanged(int, int, int);
    void _documentOpened(Document* document);
    void _documentSaved();

    void setRunTrigger(int runTrigger);

signals:
    void nameChanged();
    void runTriggerChanged();

    void runError(QJSValue errors);
    void objectReady();

private:
    void runLv();

    QObject *createObject(const QByteArray& code, const QUrl& file, QQmlContext* context);
    QQmlContext* createContext();
    void emptyRunSpace();

    QString               m_name;
    QString               m_path;
    QQmlComponent*        m_component;
    QObject*              m_runSpace;
    ViewEngine*           m_viewEngine;
    QObject*              m_viewRoot;
    QQmlContext*          m_viewContext;
    Type                  m_type;
    QSet<QString>         m_activations;
    Project*              m_project;
    QTimer*               m_scheduleTimer;
    int                   m_runTrigger;
    el::Engine*           m_engine;
    el::Element*          m_runtimeRoot;
};

inline const QString &Runnable::name() const{
    return m_name;
}

inline void Runnable::setName(const QString &name){
    if (m_name == name)
        return;

    m_name = name;
    emit nameChanged();
}

inline QObject *Runnable::runSpace(){
    return m_runSpace;
}

inline QObject *Runnable::viewRoot(){
    return m_viewRoot;
}

inline const QSet<QString> &Runnable::activations() const{
    return m_activations;
}

inline const QString &Runnable::path() const{
    return m_path;
}

inline int Runnable::runTrigger() const{
    return m_runTrigger;
}

inline Runnable::Type Runnable::type() const{
    return m_type;
}

}// namespace

#endif // LVRUNNABLE_H
