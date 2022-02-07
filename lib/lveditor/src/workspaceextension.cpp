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

#include "workspaceextension.h"

/**
 * \class lv::WorkspaceExtension
 * \brief The class within Qml to be used when we want to add an extension
 * \ingroup lvview
 */
namespace lv{

/** Default contructor */
WorkspaceExtension::WorkspaceExtension(QObject *parent)
    : QObject(parent)
    , m_globals(nullptr)
{
}

/** Returns true if a file interceptor has been set */
bool WorkspaceExtension::hasFileInterceptor() const{
    return m_interceptFile.isCallable();
}

/** Calls the file interceptor with the given values as args */
QJSValue WorkspaceExtension::callFileInterceptor(const QJSValueList &values){
    QJSValue result = m_interceptFile.call(values);
    if ( result.isError() ){
        qWarning("File intercept error: %s", qPrintable(result.toString()));
        return QJSValue();
    }

    return result;
}

/** Shows if we have a language interceptor */
bool WorkspaceExtension::hasLanguageInterceptor() const{
    return m_interceptLanguage.isCallable();
}

/** Calls the language interceptor on the set of given values */
QObject *WorkspaceExtension::callLanguageInterceptor(const QJSValueList& values){
    QJSValue result = m_interceptLanguage.call(values);
    if ( result.isError() ){
        qWarning("Language Intercept Error: %s", qPrintable(result.toString()));
        return nullptr;
    }

    return result.toQObject();
}

/** Shows if we have a menu interceptor */
bool WorkspaceExtension::hasMenuInterceptors() const{
    return m_menuInterceptors.isArray();
}

/** Calls the menu interceptor on the given set of values */
QJSValue WorkspaceExtension::callMenuInterceptors(const QJSValueList &values){
    QJSValue result = m_menuInterceptors.call(values);
    if ( result.isError() ){
        qWarning("Menu Intercept Error: %s", qPrintable(result.toString()));
        return QJSValue();
    }

    return result;
}

}// namespace

