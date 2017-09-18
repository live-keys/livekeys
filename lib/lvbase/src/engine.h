/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#ifndef LVENGINE_H
#define LVENGINE_H

#include <QObject>
#include <QJSValue>
#include <QMap>
#include <functional>

#include "live/lvbaseglobal.h"
#include "live/exception.h"

class QQmlEngine;
class QQmlError;
class QQmlIncubator;
class QMutex;

namespace lcv{

class ErrorHandler;
class IncubationController;

class LVBASE_EXPORT FatalException : public lcv::Exception{

public:
    FatalException(const QString& message, int code = 0): lcv::Exception(message, code){}
    virtual ~FatalException(){}
};

class LVBASE_EXPORT InputException : public lcv::Exception{
public:
    InputException(const QString& message, int code = 0) : lcv::Exception(message, code){}
    virtual ~InputException(){}
};


class LVBASE_EXPORT Engine : public QObject{

    Q_OBJECT
    Q_PROPERTY(bool isLoading READ isLoading NOTIFY isLoadingChanged)

public:
    explicit Engine(QQmlEngine* engine, QObject *parent = 0);
    ~Engine();

    bool isLoading() const;
    void setIsLoading(bool isLoading);

    void useEngine(std::function<void(QQmlEngine*)> call);

    const QList<QQmlError>& lastErrors() const;

    QQmlEngine* engine();
    QMutex* engineMutex();

    QJSValue evaluate(const QString& jsCode, const QString &fileName = QString(), int lineNumber = 1);
    void throwError(const Exception *e, QObject* object = 0);
    void throwError(const QQmlError& error);

    void throwWarning(const QString& message, QObject* object = 0, const QString& fileName = QString(), int lineNumber = 0);

    bool hasErrorHandler(QObject* object);
    void registerErrorHandler(QObject* object, ErrorHandler* handler);
    void removeErrorHandler(QObject* object);

signals:
    void aboutToCreateObject(const QUrl& file);
    void isLoadingChanged(bool isLoading);
    void objectCreated(QObject* object);
    void objectCreationError(QJSValue errors);

    void applicationError(QJSValue error);
    void applicationWarning(QJSValue warning);

public slots:
    void createObjectAsync(
        const QString& qmlCode,
        QObject* parent,
        const QUrl& file,
        bool clearCache = false
    );
    QObject* createObject(const QString& qmlCode, QObject* parent, const QUrl& file, bool clearCache = false);
    void engineWarnings(const QList<QQmlError>& warnings);

    void throwError(const QJSValue& error, QObject* object = 0);
    void throwWarning(const QJSValue& error, QObject* object = 0);
    QString markErrorObject(QObject* object);
    QJSValue lastErrorsObject() const;

private:
    QJSValue toJSError(const QQmlError& error) const;
    QJSValue toJSErrors(const QList<QQmlError>& errors) const;

    QQmlEngine*    m_engine;
    QMutex*        m_engineMutex;
    QQmlIncubator* m_incubator;
    IncubationController* m_incubationController;
    QJSValue       m_errorType;
    //TODO: Add std::function analyzer

    QList<QQmlError>              m_lastErrors;
    QMap<QObject*, ErrorHandler*> m_errorHandlers;
    QMap<QString,  QObject*>      m_errorObjects;

    bool m_isLoading;
};

inline bool Engine::isLoading() const{
    return m_isLoading;
}

inline void Engine::setIsLoading(bool isLoading){
    m_isLoading = isLoading;
}

inline QQmlEngine*Engine::engine(){
    return m_engine;
}

inline QMutex *Engine::engineMutex(){
    return m_engineMutex;
}

}// namespace

#endif // LVENGINE_H
