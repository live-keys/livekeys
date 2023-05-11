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
#include "qmlmetaextension.h"
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

void logJsValue(VisualLog& vl, const QJSValue& message, ViewEngine* ve){
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
                logJsValue(vl, vi.value(), ve);
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
                    logJsValue(vl, vi.value(), ve);
                }
            }
            vl << "}";
        }
    } else {
        vl << message.toString();
    }
}

void logHelper(VisualLog::MessageInfo::Level level, const QJSValue& messageOrCategory, const QJSValue& message, ViewEngine* ve){
    if ( message.isUndefined() ){
        VisualLog vl(level);
        QmlVisualLog::logValue(vl, messageOrCategory, ve);
    } else {
        VisualLog vl(messageOrCategory.toString().toStdString(), level);
        QmlVisualLog::logValue(vl, message, ve);
    }
}

}// namespace


void QmlVisualLog::logValue(VisualLog& vl, const QJSValue& message, ViewEngine *engine){
    if ( message.isQObject() ){
        QObject* messageObject = message.toQObject();
        QmlExtensionObjectI* eo = dynamic_cast<QmlExtensionObjectI*>(messageObject);
        bool userLog = false;
        if ( eo && engine ){
            MetaLogI* mli = eo->typeMetaExtension()->i<MetaLogI>();
            if ( mli ){
                userLog = true;
                mli->log(engine, messageObject, vl);
            }
        }
        if ( !userLog ){
        //TODO
//        QmlMetaInfo::Ptr ti = ViewContext::instance().engine()->typeInfo(messageObject->metaObject());
//        if ( !ti.isNull() && ti->isLoggable() ){
//            ti->log(vl, messageObject);
//        } else {
            vl << "(" << messageObject->metaObject()->className() << " " <<
               (messageObject->objectName().isEmpty() ? "" : "\'" + messageObject->objectName() + "\' ") <<
                messageObject << ")";
        }
    } else {
        logJsValue(vl, message, engine);
    }
}

/** Default constructor */
QmlVisualLog::QmlVisualLog(ViewEngine *parent)
    : QObject(parent)
    , m_engine(parent)
{
}

/** Default destructor */
QmlVisualLog::~QmlVisualLog(){
}

/** Fatal messages */
void QmlVisualLog::f(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Fatal, messageOrCategory, message, m_engine);
}

/** Error messages */
void QmlVisualLog::e(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Error, messageOrCategory, message, m_engine);
}

/** Warning messages */
void QmlVisualLog::w(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Warning, messageOrCategory, message, m_engine);
}

/** Info messages */
void QmlVisualLog::i(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Info, messageOrCategory, message, m_engine);
}

/** Debug messages */
void QmlVisualLog::d(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Debug, messageOrCategory, message, m_engine);
}

/** Verbose messages */
void QmlVisualLog::v(const QJSValue &messageOrCategory, const QJSValue &message){
    logHelper(VisualLog::MessageInfo::Verbose, messageOrCategory, message, m_engine);
}

/** Configures global vlog object from a given QJSValue object */
void QmlVisualLog::configure(const QString &name, const QJSValue &options){
    try{
        MLNode mlopt;
        ml::fromQml(options, mlopt);
        vlog().configure(name.toStdString(), mlopt);
    } catch ( lv::Exception& e ){
        auto en = qobject_cast<QQmlEngine*>(parent());
        auto ve = ViewEngine::grabFromQmlEngine(en);
        ve->throwError(&e, this);
    }
}

}// namespace
