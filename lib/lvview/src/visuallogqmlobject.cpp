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

#include "visuallogqmlobject.h"
#include "live/visuallog.h"
#include "mlnodetoqml.h"
#include "live/viewcontext.h"
#include "visuallogqt.h"
#include "typeinfo.h"
#include "viewengine.h"

#include <QMetaType>

namespace lv{

namespace{

void logHelper(VisualLog::MessageInfo::Level level, const QJSValue& messageOrCategory, const QJSValue& message){
    if ( message.isUndefined() ){
        if ( messageOrCategory.isQObject() ){
            QObject* messageObject = messageOrCategory.toQObject();
            TypeInfo::Ptr ti = ViewContext::instance().engine()->typeInfo(messageObject->metaObject());
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
            TypeInfo::Ptr ti = ViewContext::instance().engine()->typeInfo(messageObject->metaObject());
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

VisualLogQmlObject::VisualLogQmlObject(QObject *parent)
    : QObject(parent)
{
}

VisualLogQmlObject::~VisualLogQmlObject(){
}

void VisualLogQmlObject::f(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Fatal, messageOrCategory, message);
}

void VisualLogQmlObject::e(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Error, messageOrCategory, message);
}
void VisualLogQmlObject::w(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Warning, messageOrCategory, message);
}
void VisualLogQmlObject::i(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Info, messageOrCategory, message);
}
void VisualLogQmlObject::d(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Debug, messageOrCategory, message);
}
void VisualLogQmlObject::v(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Verbose, messageOrCategory, message);
}

void VisualLogQmlObject::configure(const QString &name, const QJSValue &options){
    MLNode mlopt;
    ml::fromQml(options, mlopt);
    vlog().configure(name.toStdString(), mlopt);
}

}// namespace
