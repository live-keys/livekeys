/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
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

#include "live/errorhandler.h"
#include "live/engine.h"
#include <QQmlEngine>
#include <QQmlContext>

#include <QDebug>

namespace lv{

ErrorHandler::ErrorHandler(QObject *parent)
    : QObject(parent)
    , m_engine(0)
    , m_target(parent)
    , m_componentComplete(false)
{
}

ErrorHandler::~ErrorHandler(){
    if ( m_engine && m_target ){
        m_engine->removeEventFilter(m_target);
    }
}

void ErrorHandler::componentComplete(){
    m_componentComplete = true;
    if ( !m_target ){
        m_target = parent();
        if ( !m_target )
            return;
    }

    QObject* livecv  = qmlEngine(this)->rootContext()->contextProperty("livecv").value<QObject*>();
    if ( !livecv ){
        qCritical("ErrorHandler: Failed to find engine. ErrorHandling will not work.");
        return;
    }
    m_engine = qobject_cast<Engine*>(livecv->property("engine").value<QObject*>());
    if ( !m_engine ){
        qCritical("ErrorHandler: Failed to find engine. ErrorHandling will not work.");
    }

    m_engine->registerErrorHandler(m_target, this);
}

void ErrorHandler::signalError(const QJSValue &err){
    if ( err.property("name").toString() == "FatalException" ){
        emit fatal(err);
    } else {
        emit error(err);
    }
}

void ErrorHandler::signalWarning(const QJSValue &error){
    emit warning(error);
}

void ErrorHandler::skip(const QJSValue &error){
    if ( m_engine ){
        if ( error.property("type").toString() == "Warning" ){
            m_engine->throwWarning(error, m_target ? m_target->parent() : 0);
        } else {
            m_engine->throwError(error, m_target ? m_target->parent() : 0);
        }
    }
}

void ErrorHandler::setTarget(QObject *target){
    if (m_target == target)
        return;

    if ( m_target && m_engine && m_componentComplete ){
        m_engine->removeErrorHandler(m_target);
    }

    m_target = target;
    emit targetChanged(target);
}

}// namespace
