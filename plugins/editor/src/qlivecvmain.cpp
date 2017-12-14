/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "qlivecvmain.h"
#include "qenginemonitor.h"
#include "qscriptcommandlineparser_p.h"

#include <QException>
#include <QQuickView>
#include <QQmlContext>
#include <QVariant>
#include <QQuickWindow>
#include <QQmlEngine>
#include <QJSValueIterator>


/*!
   \class lv::QLiveCVMain
   \inmodule live_cpp
   \internal
 */

/*!
  \qmltype Main
  \instantiates lcv::QLiveCVMain
  \inqmlmodule live
  \inherits Item
  \brief Provides a main script entry to a qml application.
  \quotefile live/mainscript.qml
 */

/*!
  \qmlproperty array Main::options

  Configuration options to parse received arguments. There are two types of options. Value types and flag
  types. Flags are simply checked to see whether they have been set within Live CV.

  \qml
  Main{
      id: main
      options : [
        { key: ['-f', '--someflag'], describe: 'Enables a feature.' }
      ]
  }
  \endqml

  Then we can check if the option has been set by querying our Main object:

  \code
  console.log('Flag status: ' + main.isOptionSet('-f'))
  \endcode

  Value types need an extra 'type' key and an optional 'required' key, which by default is false:

  \qml
  Main{
      id: main
      options : [
        { key: ['-v', '--value'], type: 'string', describe: 'Receive value.', required: true }
      ]
  }
  \endqml

  The type can be anything used to intuitively describe the value. It does not have to match a qml type.
  If an option is required and is missing when Live CV is called, then Live CV will exit, and a help text will be
  displayed to the user.

 */

/*!
  \qmlproperty string Main::version

  Sets the script version.
 */

/*!
  \qmlsignal Main::run()

  Emited once the arguments are parsed and the script is ready to run.
 */

QLiveCVMain::QLiveCVMain(QQuickItem *parent)
    : QQuickItem(parent)
    , m_parser(0)
{
    setFlag(ItemHasContents, false);
}

QLiveCVMain::~QLiveCVMain(){
}

void QLiveCVMain::componentComplete(){
    QQuickItem::componentComplete();

    QEngineMonitor* em = QEngineMonitor::grabFromContext(this);

    connect(em, SIGNAL(afterCompile()), this, SLOT(afterCompile()));
    connect(em, SIGNAL(beforeCompile()), this, SLOT(beforeCompile()));

    QObject* obj = qmlContext(this)->contextProperty("script").value<QObject*>();
    m_parser = new QScriptCommandLineParser(obj->property("argvTail").toStringList());
}

void QLiveCVMain::beforeCompile(){
    // Remove before a new compilation takes place (otherwise aftercompile triggers twice)
    disconnect(sender(), SIGNAL(afterCompile()), this, SLOT(afterCompile()));
}

void QLiveCVMain::afterCompile(){
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

/*!
 \qmlmethod Main::arguments()

 Returns the arguments after extracting the parsed options

 */

const QStringList &QLiveCVMain::arguments() const{
    return m_parser->arguments();
}

/*!
  \qmlmethod Main::option(key)

  Returns the option configured at \a key. Raises a warning if the option was not configured.
 */

QString QLiveCVMain::option(const QString &key) const{
    QScriptCommandLineParser::Option* option = m_parser->findOptionByName(key);
    if ( !option ){
        qCritical("Failed to find script option: %s", qPrintable(key));
        return "";
    }
    return m_parser->value(option);
}

/*!
 \qmlmethod bool Main::isOptionSet(key)

 Returns true if the option at \a key has been set. False otherwise.
 */
bool QLiveCVMain::isOptionSet(const QString &key) const{
    QScriptCommandLineParser::Option* option = m_parser->findOptionByName(key);
    if ( !option ){
        qCritical("Failed to find script option: %s", qPrintable(key));
        return "";
    }
    return m_parser->isSet(option);
}
