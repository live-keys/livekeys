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

#include "qmlmain.h"
#include "qmlscriptargumentsparser.h"
#include "live/viewcontext.h"
#include "live/visuallogqt.h"
#include "live/viewengine.h"
#include "live/qmlerror.h"

#include <QQuickView>
#include <QQmlContext>
#include <QVariant>
#include <QQmlEngine>
#include <QJSValueIterator>

namespace lv{

QmlMain::QmlMain(QQuickItem *parent)
    : QQuickItem(parent)
    , m_engine(nullptr)
    , m_parser(nullptr)
{
}

QmlMain::~QmlMain(){
}

void QmlMain::classBegin(){
    QQuickItem::classBegin();

    m_engine = ViewEngine::grab(this);
    if ( !m_engine ){
        QmlError::warnNoEngineCaptured(this);
        return;
    }
    QObject* obj = qmlContext(this)->contextProperty("project").value<QObject*>();
    m_args = obj->property("scriptArgv").value<QJSValue>();
}

void QmlMain::componentComplete(){
    QQuickItem::componentComplete();

    if ( m_run.isCallable() ){
        QJSValue result = m_run.call();
        if ( result.isError() ){
            m_engine->throwError(result, this);
        }
    }
}

QJSValue QmlMain::parse(const QJSValue &options){
    try{
        m_parser->resetScriptOptions();

        QList<QmlScriptArgumentsParser::Option*> parserOptions;
        QList<QmlScriptArgumentsParser::Option*> requiredOptions;

        if ( !options.isArray() ){
            THROW_QMLERROR(lv::Exception, "Unknown options argument type. Expected array.", Exception::toCode("~Argument"), this);
            return QJSValue();
        }

        QJSValueIterator it(options);
        while( it.hasNext() ){
            it.next();
            if ( it.name() == "length")
                continue;
            QJSValue option = it.value();
            if ( !option.isObject() ){
                THROW_QMLERROR(lv::Exception, Utf8("Options[%] is not an object.").format(it.name()), Exception::toCode("~Argument"), this);
                return QJSValue();
            }
            if ( !option.hasOwnProperty("key") ){
                THROW_QMLERROR(lv::Exception, Utf8("Options[%] is missing key property.").format(it.name()), Exception::toCode("~Argument"), this);
                return QJSValue();
            }

            QStringList keys;
            QJSValue jsKeys = option.property("key");
            if ( jsKeys.isArray() ){
                QJSValueIterator jsKeysIt(jsKeys);
                while ( jsKeysIt.hasNext() ){
                    jsKeysIt.next();
                    if ( jsKeysIt.name() != "length" )
                        keys << jsKeysIt.value().toString();
                }
            } else {
                keys << jsKeys.toString();
            }

            QString type = option.hasOwnProperty("type") ? option.property("type").toString() : "";
            QString describe = option.hasOwnProperty("describe") ? option.property("describe").toString() : "";

            if ( type == "" ){
                auto parserOpt = m_parser->addFlag(keys, describe);
                parserOptions.append(parserOpt);
            } else {
                auto parserOpt = m_parser->addOption(keys, describe, type);
                parserOptions.append(parserOpt);
                bool required = option.hasOwnProperty("required") ? option.property("required").toBool() : false;
                if ( required )
                    requiredOptions.append(parserOpt);
            }
        }

        m_parser->parseArguments();

        if ( m_parser->isSet(m_parser->helpOption())){
            QJSValue helpOpt = m_engine->engine()->newObject();
            helpOpt.setProperty("help", true);
            helpOpt.setProperty("helpText", m_parser->helpString());
            return helpOpt;
        } else if ( m_parser->isSet(m_parser->versionOption() ) ){
            QJSValue versionOpt = m_engine->engine()->newObject();
            versionOpt.setProperty("version", true);
            versionOpt.setProperty("versionText", m_parser->helpString());
            return versionOpt;
        }

        for( QmlScriptArgumentsParser::Option* option : requiredOptions ){
            if ( !m_parser->isSet(option) ){
                THROW_QMLERROR(lv::Exception, Utf8("Failed to find required option: %").format(m_parser->optionNames(option).join(' ')), lv::Exception::toCode("~Option"), this);
                return QJSValue();
            }
        }

        QJSValue result = m_engine->engine()->newObject();
        for( QmlScriptArgumentsParser::Option* option : parserOptions ){
            QStringList names = m_parser->optionNames(option);
            QJSValue val = m_engine->engine()->newObject();
            val.setProperty("isSet", m_parser->isSet(option));
            val.setProperty("value", m_parser->value(option));
            for ( QString name : names ){
                result.setProperty(name, val);
            }
        }

        return result;

    } catch ( lv::Exception& e ){
        m_engine->throwError(&e, this);
        return QJSValue();
    }

    return QJSValue();
}

QString QmlMain::parserHelpString() const{
    return m_parser->helpString();
}

void QmlMain::exit(int code){
    QObject* projectInfo = qmlContext(this)->contextProperty("project").value<QObject*>();
    if ( projectInfo ){
        QMetaObject::invokeMethod(projectInfo, "exit", Q_ARG(int, code));
    }
}

void QmlMain::setVersion(const QString &version){
    if (m_version == version)
        return;
    if ( isComponentComplete() ){
        THROW_QMLERROR(lv::Exception, "Cannot set version once component is complete.", lv::Exception::toCode("Complete"), this);
        return;
    }

    m_version = version;
    emit versionChanged();
}

void QmlMain::setRun(const QJSValue &arg){
    if ( isComponentComplete() ){
        THROW_QMLERROR(lv::Exception, "Cannot set run once component is complete.", lv::Exception::toCode("Complete"), this);
        return;
    }
    if ( !arg.isCallable() ){
        THROW_QMLERROR(lv::Exception, "Run arguments needs to be a runnable.", lv::Exception::toCode("~Callable"), this);
        return;
    }

    m_run = arg;
    emit runChanged();
}

}// namespace
