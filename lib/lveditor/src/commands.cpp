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
#include "live/visuallog.h"
#include "live/visuallogqt.h"
#include <QJSValueIterator>
#include <QDebug>

namespace lv{

Commands::Commands(QObject *parent)
    : QObject(parent)
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
    if ( object == 0 || object->objectName() == "" ){
        qCritical("Cannot add commands for unnamed objects.");
        return "";
    }
    if ( !commands.isObject() ){
        qCritical("Commands requires to be of object type.");
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
                        qCritical("First value in array isn't a function: %s", qPrintable(key));
                        break;
                    }
                }
                if (i == 1) {
                    if (val.isString())
                    {
                        n->description = val.toString();
                    } else {
                        qCritical("Second value in array isn't a string: %s", qPrintable(key));
                        break;
                    }
                }
                if (i == 2) {
                    if (val.isCallable())
                    {
                        n->check = val;
                    } else {
                        qCritical("Third value in array isn't a function: %s", qPrintable(key));
                        break;
                    }
                }
                if (i > 2) break;
            }
        }
        else {
            qCritical("Value given for command is neither a function nor an array: %s", qPrintable(key));
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
    while( object != 0 ){
        if ( object->objectName() != "" )
            commandChain.prepend(object->objectName());
        object = object->parent();
    }

    return commandChain;
}

void Commands::execute(const QString &command){
    auto it = m_commands.find(command);
    if ( it != m_commands.end() ){
        QJSValue r = it.value()->function.call();
        if ( r.isError() ){
            qWarning("Error executing command %s: %s", qPrintable(command), qPrintable(r.toString()));
        }
        return;
    }
}

}// namespace
