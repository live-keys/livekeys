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

#include "visuallogjsobject.h"
#include "visuallog.h"
#include "mlnodetojs.h"
#include "plugincontext.h"
#include "typeinfo.h"
#include "engine.h"

#include <QMetaType>

namespace lv{

namespace{

void log_helper(VisualLog::MessageInfo::Level level, const QJSValue& messageOrCategory, const QJSValue& message){
    if ( message.isUndefined() ){
        if ( messageOrCategory.isQObject() ){
            QObject* messageObject = messageOrCategory.toQObject();
            TypeInfo::Ptr ti = PluginContext::engine()->typeInfo(messageObject->metaObject());
            if ( !ti.isNull() && ti->isLoggable() ){
                VisualLog vl(level);
                ti->log(vl, messageObject);
            } else {
                VisualLog(level) << "[Object object]";
            }
        } else {
            VisualLog(level) << messageOrCategory.toString();
        }
    } else {
        if ( message.isQObject() ){
            QObject* messageObject = messageOrCategory.toQObject();
            TypeInfo::Ptr ti = PluginContext::engine()->typeInfo(messageObject->metaObject());
            if ( !ti.isNull() && ti->isLoggable() ){
                VisualLog vl(level);
                ti->log(vl, messageObject);
            } else {
                VisualLog(level) << "[Object object]";
            }
        } else {
            VisualLog(level) << message.toString();
        }
    }
}

}// namespace

VisualLogJsObject::VisualLogJsObject(QObject *parent)
    : QObject(parent)
{
}

VisualLogJsObject::~VisualLogJsObject(){
}

void VisualLogJsObject::f(const QJSValue &messageOrCategory, const QJSValue &message){
    log_helper(VisualLog::MessageInfo::Fatal, messageOrCategory, message);
}

void VisualLogJsObject::e(const QJSValue &messageOrCategory, const QJSValue &message){
    log_helper(VisualLog::MessageInfo::Error, messageOrCategory, message);
}
void VisualLogJsObject::w(const QJSValue &messageOrCategory, const QJSValue &message){
    log_helper(VisualLog::MessageInfo::Warning, messageOrCategory, message);
}
void VisualLogJsObject::i(const QJSValue &messageOrCategory, const QJSValue &message){
    log_helper(VisualLog::MessageInfo::Info, messageOrCategory, message);
}
void VisualLogJsObject::d(const QJSValue &messageOrCategory, const QJSValue &message){
    log_helper(VisualLog::MessageInfo::Debug, messageOrCategory, message);
}
void VisualLogJsObject::v(const QJSValue &messageOrCategory, const QJSValue &message){
    log_helper(VisualLog::MessageInfo::Verbose, messageOrCategory, message);
}

void VisualLogJsObject::configure(const QString &name, const QJSValue &options){
    MLNode mlopt;
    ml::fromJs(options, mlopt);
    vlog().configure(name, mlopt);
}

}// namespace
