#include "qlivecvmain.h"
#include "qlivecvarguments.h"
#include "qlivecvcommandlineparser.h"

#include <QQuickView>
#include <QQmlContext>
#include <QVariant>
#include <QQuickWindow>
#include <QQmlEngine>
#include <QJSValueIterator>

namespace lcv{

QLiveCVMain::QLiveCVMain(QQuickItem *parent)
    : QQuickItem(parent)
    , m_attachedWindow(0)
{
    setFlag(ItemHasContents, false);
    connect(this, SIGNAL(windowChanged(QQuickWindow*)), SLOT(attachWindow(QQuickWindow*)));
}

QLiveCVMain::~QLiveCVMain(){
}

void QLiveCVMain::attachWindow(QQuickWindow *window){
    if (window == 0){
        qWarning("Cannot connect to window compilation signal. , \'Main\' object will not run.");
        return;
    }

    connect(window, SIGNAL(aboutToRecompile()), this, SLOT(aboutToRecompile()));
    connect(window, SIGNAL(afterCompile()), this, SLOT(afterCompile()));
    m_attachedWindow = window;
}

void QLiveCVMain::aboutToRecompile(){
    disconnect(m_attachedWindow, SIGNAL(afterCompile()), this, SLOT(afterCompile()));
}

void QLiveCVMain::afterCompile(){
    if ( !m_attachedWindow )
        return;

    QObject* obj = QQmlEngine::contextForObject(this)->contextProperty("args").value<QObject*>();
    QLiveCVArguments* argsOb = qobject_cast<QLiveCVArguments*>(obj);
    if ( !argsOb ){
        qWarning("Failed to fetch arguments object. \'Main\' object will not run.");
        return;
    }

    try{

        argsOb->parser()->resetScriptOptions();

        QList<QLiveCVCommandLineParser::Option*> requiredOptions;

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
                argsOb->parser()->addScriptFlag(keys, describe);
            } else {
                QLiveCVCommandLineParser::Option* parserOpt = argsOb->parser()->addScriptOption(keys, describe, type);
                bool required = option.hasOwnProperty("required") ? option.property("required").toBool() : false;
                if ( required )
                    requiredOptions.append(parserOpt);
            }
        }

        argsOb->parser()->parseScriptArguments();

        if ( argsOb->parser()->isSet(argsOb->parser()->scriptHelpOption())){
            printf("%s", qPrintable(argsOb->parser()->scriptHelpString()));
            QCoreApplication::exit(0);
            return;
        } else if ( argsOb->parser()->isSet(argsOb->parser()->versionOption() ) ){
            printf("%s", qPrintable(m_version));
            QCoreApplication::exit(0);
            return;
        }

        foreach( QLiveCVCommandLineParser::Option* option, requiredOptions ){
            if ( !argsOb->parser()->isSet(option) ){
                qCritical("Failed to find required option: %s", qPrintable(argsOb->parser()->optionNames(option).first()));
                printf("%s", qPrintable(argsOb->parser()->scriptHelpString()));
                return;
            }
        }

        emit run();

    } catch ( QLiveCVCommandLineException& e ){
        qCritical("Command line option error: %s", qPrintable(e.message()));
    }
}

}// namespace
