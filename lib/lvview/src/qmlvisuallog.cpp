/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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

#include "qmlvisuallog.h"
#include "live/visuallog.h"
#include "mlnodetoqml.h"
#include "live/viewcontext.h"
#include "visuallogqt.h"
#include "metainfo.h"
#include "viewengine.h"

#include <QJSValueIterator>
#include <QMetaType>

/**
 * \class lv::VisualLogQmlObject
 * \brief Contains methods to present a Qml output for the visual log
 * \ingroup lvview
 */
namespace lv{

namespace{

void logJsValue(VisualLog& vl, const QJSValue& message){
    if ( message.isQObject() ){
        QObject* messageObject = message.toQObject();
        vl << "(" << messageObject->metaObject()->className() << " " <<
           (messageObject->objectName().isEmpty() ? "" : "\'" + messageObject->objectName() + "\' ") <<
            messageObject << ")";
    } else if ( message.isArray() ){
        vl << "[";
        bool first = true;
        QJSValueIterator vi(message);
        while ( vi.next() ){
            if ( vi.name() != "length" ){
                if (first)
                    first = false;
                else
                    vl << ",";
                logJsValue(vl, vi.value());
            }
        }
        vl << "]";
    } else if ( message.isCallable() ){
        vl << message.toString();
    } else if ( message.isObject() ){
        if ( message.hasProperty("byteLength") ){ // ArrayBuffer
            vl << message.toString();
        } else {
            vl << "{";
            bool first = true;
            QJSValueIterator vi(message);
            while ( vi.next() ){
                if ( message.hasOwnProperty(vi.name()) ){
                    if (first)
                        first = false;
                    else
                        vl << ",";
                    vl << vi.name() << ":";
                    logJsValue(vl, vi.value());
                }
            }
            vl << "}";
        }
    } else {
        vl << message.toString();
    }
}

void logHelper(VisualLog::MessageInfo::Level level, const QJSValue& messageOrCategory, const QJSValue& message){
    if ( message.isUndefined() ){
        VisualLog vl(level);
        QmlVisualLog::logValue(vl, messageOrCategory);
    } else {
        VisualLog vl(messageOrCategory.toString().toStdString(), level);
        QmlVisualLog::logValue(vl, message);
    }
}

}// namespace


void QmlVisualLog::logValue(VisualLog& vl, const QJSValue& message){
    if ( message.isQObject() ){
        QObject* messageObject = message.toQObject();
        MetaInfo::Ptr ti = ViewContext::instance().engine()->typeInfo(messageObject->metaObject());
        if ( !ti.isNull() && ti->isLoggable() ){
            ti->log(vl, messageObject);
        } else {
            vl << "(" << messageObject->metaObject()->className() << " " <<
               (messageObject->objectName().isEmpty() ? "" : "\'" + messageObject->objectName() + "\' ") <<
                messageObject << ")";
        }
    } else {
        logJsValue(vl, message);
    }
}

/** Default constructor */
QmlVisualLog::QmlVisualLog(QObject *parent)
    : QObject(parent)
{
}

/** Default destructor */
QmlVisualLog::~QmlVisualLog(){
}

/** Fatal messages */
void QmlVisualLog::f(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Fatal, messageOrCategory, message);
}

/** Error messages */
void QmlVisualLog::e(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Error, messageOrCategory, message);
}

/** Warning messages */
void QmlVisualLog::w(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Warning, messageOrCategory, message);
}

/** Info messages */
void QmlVisualLog::i(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Info, messageOrCategory, message);
}

/** Debug messages */
void QmlVisualLog::d(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Debug, messageOrCategory, message);
}

/** Verbose messages */
void QmlVisualLog::v(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Verbose, messageOrCategory, message);
}

/** Configures global vlog object from a given QJSValue object */
void QmlVisualLog::configure(const QString &name, const QJSValue &options){
    MLNode mlopt;
    ml::fromQml(options, mlopt);
    vlog().configure(name.toStdString(), mlopt);
}

}// namespace
