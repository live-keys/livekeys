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

#include "commands.h"
#include "workspacelayer.h"
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include "live/viewengine.h"

#include <QJSValueIterator>
#include <QDebug>

namespace lv{

Commands::Commands(ViewEngine *engine, QObject *parent)
    : QObject(parent)
    , m_engine(engine)
{
}

Commands::~Commands(){
    for ( auto it = m_commands.begin(); it != m_commands.end(); ++it ){
        delete it.value();
    }
}

QString Commands::dump(){
    QString result;
    for ( auto it = m_commands.begin(); it != m_commands.end(); ++it ){
        result += it.key() + "\n";
    }
    return result;
}

void Commands::setModel(CommandsModel *m)
{
    m_model = m;
    connect(m_model, &CommandsModel::modelChanged, this, &Commands::modelChanged);
}

QString Commands::add(QObject *object, const QJSValue &commands){
    if ( !m_engine )
        THROW_EXCEPTION(lv::Exception, "Engine not assigned.", Exception::toCode("~Engine"));

    if ( object == nullptr || object->objectName() == "" ){
        QmlError(m_engine, CREATE_EXCEPTION(lv::Exception, "Cannot add commands for unnamed objects.", Exception::toCode("~Commands")), this).jsThrow();
        return "";
    }
    if ( !commands.isObject() ){
        QmlError(m_engine, CREATE_EXCEPTION(lv::Exception, "Argument 'commands' requires to be of object type.", Exception::toCode("~Commands")), this).jsThrow();
        return "";
    }

    QStringList commandChain = getCommandChain(object);
    QString prefix = commandChain.join(".");

    vlog("commmands").v() << "Adding commands for prefix: " << prefix;

    QJSValueIterator vit(commands);
    while( vit.hasNext() ){
        vit.next();

        QString key = prefix + "." + vit.name();

        if ( vit.value().isCallable() ){

            auto it = m_commands.find(key);

            if ( it == m_commands.end() ){
                Commands::Node* n = new Commands::Node;
                n->function = vit.value();

                m_commands.insert(key, n);
            }
        }
        else if (vit.value().isArray()) {
            QJSValue arr = vit.value();
            unsigned len = static_cast<unsigned>(arr.property("length").toInt());

            Commands::Node* n = nullptr;

            auto it = m_commands.find(key);

            if ( it == m_commands.end() ){
                n = new Commands::Node;
                m_commands.insert(key, n);
            } else {
                n = it.value();
            }

            for (unsigned int i = 0; i <len; i++)
            {
                QJSValue val = arr.property(i);
                if (i == 0) {
                    if (val.isCallable())
                    {
                        n->function = val;
                    } else
                    {
                        QmlError(m_engine, CREATE_EXCEPTION(lv::Exception, "First value in array isn't a function: " + key.toStdString() + ".", Exception::toCode("~Commands")), this).jsThrow();
                        break;
                    }
                }
                if (i == 1) {
                    if (val.isString())
                    {
                        n->description = val.toString();
                    } else {
                        QmlError(m_engine, CREATE_EXCEPTION(lv::Exception, "Second value in array isn't a string: " + key.toStdString() + ".", Exception::toCode("~Commands")), this).jsThrow();
                        break;
                    }
                }
                if (i == 2) {
                    if (val.isCallable())
                    {
                        n->check = val;
                    } else {
                        QmlError(m_engine, CREATE_EXCEPTION(lv::Exception, "Third value in array isn't a function: " + key.toStdString() + ".", Exception::toCode("~Commands")), this).jsThrow();
                        break;
                    }
                }
                if (i > 2) break;
            }
        }
        else {
            QmlError(m_engine, CREATE_EXCEPTION(lv::Exception, "Value given for command is neither a function nor an array: " + key.toStdString() + ".", Exception::toCode("~Commands")), this).jsThrow();
        }
    }
    m_model->updateAvailableCommands();

    return prefix;
}

void Commands::removeCommandsFor(QObject *object){
    QStringList commandChain = getCommandChain(object);
    if ( commandChain.isEmpty() )
        return;

    QString commandName = commandChain.join(".");

    auto it = m_commands.begin();
    while ( it != m_commands.end() ){
        if ( it.key().startsWith(commandName) ){
            delete it.value();
            it = m_commands.erase(it);
        } else {
            ++it;
        }
    }
}

QStringList Commands::getCommandChain(QObject *object){
    QStringList commandChain;
    while( object != nullptr ){
        if ( object->objectName() != "" )
            commandChain.prepend(object->objectName());
        object = object->parent();
    }

    return commandChain;
}

void Commands::execute(const QString &command){
    if ( !m_engine )
        THROW_EXCEPTION(lv::Exception, "Engine not assigned.", Exception::toCode("~Engine"));

    auto it = m_commands.find(command);
    if ( it != m_commands.end() ){
        QJSValue r = it.value()->function.call();
        if ( r.isError() ){
            m_engine->throwError(r, this);
//            QmlError(m_engine, CREATE_EXCEPTION(lv::Exception, "Error executing command '" + command.toStdString() + "': " + r.toString().toStdString() + ".", Exception::toCode("~Commands")), this).jsThrow();
        }
        return;
    }
}

}// namespace
