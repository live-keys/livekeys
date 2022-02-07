/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
** This file is part of Livekeys Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVRUNNABLE_H
#define LVRUNNABLE_H

#include "live/lveditorglobal.h"
#include "live/projectdocument.h"

#include <QObject>
#include <QQmlComponent>

namespace lv{

class ViewEngine;
class RunnableContainer;
class QmlBuild;
class Program;
class QmlProgram;

namespace el{
    class Engine;
    class Element;
}

class LV_EDITOR_EXPORT Runnable : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString name   READ name       WRITE setName       NOTIFY nameChanged)
    Q_PROPERTY(int runTrigger READ runTrigger WRITE setRunTrigger NOTIFY runTriggerChanged)
    Q_PROPERTY(QString path   READ path CONSTANT)
    Q_PROPERTY(QUrl url       READ url  CONSTANT)

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
        Program* program,
        RunnableContainer* parent,
        const QSet<QString>& activations = QSet<QString>()
    );

    Runnable(
        ViewEngine* engine,
        QQmlComponent* component,
        RunnableContainer* parent,
        const QString& name = "");

    ~Runnable();

    const QString& name() const;
    void setName(const QString& name);
    const QString& path() const;
    QUrl url() const;

    QObject* viewRoot();
    QQmlContext* viewContext();
    Program* program() const;

    el::Element* elementRoot();

    const QSet<QString>& activations() const;


    int runTrigger() const;

    Type type() const;

    QQmlContext* createContext();

    void swapViewRoot(QObject* object);

public slots:
    void __objectAcquired();
    void __objectReady(QObject* object, QQmlContext* context);
    void __objectCreationError(QJSValue errors, QQmlContext* context);

    void run();

    void setRunSpace(QObject* runspace);
    QObject* runSpace();

    void __activationContentsChanged(int, int, int);
    void __documentOpened(Document* document);
    void __documentSaved();

    void setRunTrigger(int runTrigger);
    void clearRoot();
signals:
    void nameChanged();
    void runTriggerChanged();

    void runError(QJSValue errors);
    void objectReady(QObject* object);

private:
    void initialize();
    void runLv();

    QObject *createObject(const QByteArray& code, const QUrl& file, QQmlContext* context);
    void emptyRunSpace();

    QString               m_name;
    QString               m_path;
    QmlProgram*           m_program;
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

inline QQmlContext *Runnable::viewContext(){
    return m_viewContext;
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
