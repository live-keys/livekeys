/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "live/errorhandler.h"
#include "live/viewengine.h"
#include <QQmlEngine>
#include <QQmlContext>

#include <QDebug>

/**
 * \class lv::ErrorHandler
 * \brief Object used inside QML that stops an error from passing it
 * \ingroup lvview
 */
namespace lv{

/** Default constructor */
ErrorHandler::ErrorHandler(QObject *parent)
    : QObject(parent)
    , m_engine(0)
    , m_target(parent)
    , m_componentComplete(false)
{
}

/** Default destructor */
ErrorHandler::~ErrorHandler(){
    if ( m_engine && m_target ){
        m_engine->removeEventFilter(m_target);
    }
}

/** Implementation of a QQmlParserStatus method */
void ErrorHandler::componentComplete(){
    m_componentComplete = true;
    if ( !m_target ){
        m_target = parent();
        if ( !m_target )
            return;
    }

    m_engine = ViewEngine::grab(this);
    if ( !m_engine ){
        QmlError::warnNoEngineCaptured(this);
        return;
    }

    m_engine->registerErrorHandler(m_target, this);
}


/** Used by the QML view engine to signal error */
void ErrorHandler::signalError(const QJSValue &err){
    if ( err.property("name").toString() == "FatalException" ){
        emit fatal(err);
    } else {
        emit error(err);
    }
}

/** Used by the QML view engine to signal warning */
void ErrorHandler::signalWarning(const QJSValue &error){
    emit warning(error);
}

/** Skips the error handler and propagates the error further */
void ErrorHandler::skip(const QJSValue &error){
    if ( m_engine ){
        m_engine->throwError(error, m_target ? m_target->parent() : 0);
    }
}

/** Sets the target object within the QML hierarchy that it attaches itself to */
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
