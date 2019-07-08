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

#include "qmlmain.h"
#include "qscriptcommandlineparser_p.h"
#include "live/viewcontext.h"
#include "live/viewengine.h"

#include <QException>
#include <QQuickView>
#include <QQmlContext>
#include <QVariant>
#include <QQuickWindow>
#include <QQmlEngine>
#include <QJSValueIterator>

namespace lv{

QmlMain::QmlMain(QQuickItem *parent)
    : QQuickItem(parent)
    , m_parser(0)
{
    setFlag(ItemHasContents, false);
}

QmlMain::~QmlMain(){
}

void QmlMain::componentComplete(){
    QQuickItem::componentComplete();

    connect(lv::ViewContext::instance().engine(), SIGNAL(aboutToCreateObject(QUrl)), this, SLOT(beforeCompile()));
    connect(lv::ViewContext::instance().engine(), SIGNAL(objectReady(QObject*,QUrl)), this, SLOT(afterCompile()));

    QObject* obj = qmlContext(this)->contextProperty("script").value<QObject*>();
    m_parser = new QScriptCommandLineParser(obj->property("argvTail").toStringList());
}

void QmlMain::beforeCompile(){
    // Remove before a new compilation takes place (otherwise aftercompile triggers twice)
    disconnect(sender(), SIGNAL(afterCompile()), this, SLOT(afterCompile()));
}

void QmlMain::afterCompile(){
    try{
        m_parser->resetScriptOptions();

        QList<QScriptCommandLineParser::Option*> requiredOptions;

        if ( !m_options.isArray() ){
            qCritical("Unknown options type, expected array.");
            return;
        }

        QJSValueIterator it(m_options);
        while( it.hasNext() ){
            it.next();
            if ( it.name() == "length")
                continue;
            QJSValue option = it.value();
            if ( !option.isObject() ){
                qCritical("Options[%s] is not an object.", qPrintable(it.name()));
                return;
            }
            if ( !option.hasOwnProperty("key") ){
                qCritical("\'key\' property missing from Options[%s].", qPrintable(it.name()));
                return;
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
                m_parser->addFlag(keys, describe);
            } else {
                QScriptCommandLineParser::Option* parserOpt = m_parser->addOption(keys, describe, type);
                bool required = option.hasOwnProperty("required") ? option.property("required").toBool() : false;
                if ( required )
                    requiredOptions.append(parserOpt);
            }
        }

        m_parser->parseArguments();

        if ( m_parser->isSet(m_parser->helpOption())){
            printf("%s", qPrintable(m_parser->helpString()));
            QCoreApplication::exit(0);
            return;
        } else if ( m_parser->isSet(m_parser->versionOption() ) ){
            printf("%s\n", qPrintable(m_version));
            QCoreApplication::exit(0);
            return;
        }

        foreach( QScriptCommandLineParser::Option* option, requiredOptions ){
            if ( !m_parser->isSet(option) ){
                qCritical("Failed to find required option: %s", qPrintable(m_parser->optionNames(option).first()));
                printf("%s", qPrintable(m_parser->helpString()));
                return;
            }
        }

        emit run();

    } catch ( QException& e ){
        qCritical("Command line option error: %s", e.what());
    }
}

const QStringList &QmlMain::arguments() const{
    return m_parser->arguments();
}

QString QmlMain::option(const QString &key) const{
    QScriptCommandLineParser::Option* option = m_parser->findOptionByName(key);
    if ( !option ){
        qCritical("Failed to find script option: %s", qPrintable(key));
        return "";
    }
    return m_parser->value(option);
}

bool QmlMain::isOptionSet(const QString &key) const{
    QScriptCommandLineParser::Option* option = m_parser->findOptionByName(key);
    if ( !option ){
        qCritical("Failed to find script option: %s", qPrintable(key));
        return "";
    }
    return m_parser->isSet(option);
}

}// namespace
