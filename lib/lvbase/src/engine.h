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

#ifndef QLIVECVENGINE_H
#define QLIVECVENGINE_H

#include <QObject>
#include <QJSValue>
#include <functional>

#include "live/lvbaseglobal.h"

class QQmlEngine;
class QQmlError;
class QQmlIncubator;
class QMutex;

namespace lcv{

class IncubationController;
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

signals:
    void aboutToCreateObject(const QUrl& file);
    void isLoadingChanged(bool isLoading);
    void objectCreated(QObject* object);
    void objectCreationError(QJSValue errors);

public slots:
    void createObjectAsync(
        const QString& qmlCode,
        QObject* parent,
        const QUrl& file,
        bool clearCache = false
    );
    QObject* createObject(const QString& qmlCode, QObject* parent, const QUrl& file, bool clearCache = false);

    QJSValue lastErrorsObject() const;

private:
    QJSValue toJSErrors(const QList<QQmlError>& errors) const;

    QQmlEngine*    m_engine;
    QMutex*        m_engineMutex;
    QQmlIncubator* m_incubator;
    IncubationController* m_incubationController;
    //TODO: Add std::function analyzer

    QList<QQmlError> m_lastErrors;

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

#endif // QLIVECVENGINE_H
