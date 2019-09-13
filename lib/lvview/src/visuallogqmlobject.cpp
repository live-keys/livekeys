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

#include "visuallogqmlobject.h"
#include "live/visuallog.h"
#include "mlnodetoqml.h"
#include "live/viewcontext.h"
#include "visuallogqt.h"
#include "typeinfo.h"
#include "viewengine.h"

#include <QMetaType>

/**
 * \class lv::VisualLogQmlObject
 * \brief Contains methods to present a Qml output for the visual log
 * \ingroup lvview
 */
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

/** Default constructor */
VisualLogQmlObject::VisualLogQmlObject(QObject *parent)
    : QObject(parent)
{
}

/** Default destructor */
VisualLogQmlObject::~VisualLogQmlObject(){
}

/** Fatal messages */
void VisualLogQmlObject::f(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Fatal, messageOrCategory, message);
}

/** Error messages */
void VisualLogQmlObject::e(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Error, messageOrCategory, message);
}

/** Warning messages */
void VisualLogQmlObject::w(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Warning, messageOrCategory, message);
}

/** Info messages */
void VisualLogQmlObject::i(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Info, messageOrCategory, message);
}

/** Debug messages */
void VisualLogQmlObject::d(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Debug, messageOrCategory, message);
}

/** Verbose messages */
void VisualLogQmlObject::v(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Verbose, messageOrCategory, message);
}

/** Configures global vlog object from a given QJSValue object */
void VisualLogQmlObject::configure(const QString &name, const QJSValue &options){
    MLNode mlopt;
    ml::fromQml(options, mlopt);
    vlog().configure(name.toStdString(), mlopt);
}

}// namespace
