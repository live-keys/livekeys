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

#include "scriptcommandlineparser_p.h"

namespace{

    /// \private
    std::exception makeException(const QString& str){
        return std::runtime_error(str.toStdString().c_str());
    }

}// namespace

namespace lv{

/// \private
class ScriptCommandLineParser::Option{

public:
    QStringList shortNames;
    QStringList longNames;
    QString     description;
    QString     type;
    bool        isFlag;

    bool        isSet;
    QString     value;

    Option() : isFlag(false), isSet(false){}
};

ScriptCommandLineParser::ScriptCommandLineParser(const QStringList &argvTail)
    : m_argvTail(argvTail)
{
    m_helpOption    = addFlag(QStringList() << "-h" << "--help",    "Displays this information and exits.");
    m_versionOption = addFlag(QStringList() << "-v" << "--version", "Displays version information and exits.");
}

ScriptCommandLineParser::~ScriptCommandLineParser(){
    for ( QList<ScriptCommandLineParser::Option*>::iterator it = m_options.begin(); it != m_options.end(); ++it ){
        delete *it;
    }
    m_options.clear();
}

QString ScriptCommandLineParser::helpString() const{
    QString base("\nUsage:\n\n   livekeys [...] script.qml [options] [args ...]\n\nScript options:\n\n");
    for ( QList<ScriptCommandLineParser::Option*>::const_iterator it = m_options.begin(); it != m_options.end(); ++it ){
        for ( QStringList::const_iterator nameIt = (*it)->shortNames.begin(); nameIt != (*it)->shortNames.end(); ++nameIt ){
            base += QString("  ") + "-" + *nameIt + ((*it)->type != "" ? " <" + (*it)->type + ">" : "");
        }
        for ( QStringList::const_iterator nameIt = (*it)->longNames.begin(); nameIt != (*it)->longNames.end(); ++nameIt ){
            base += QString("  ") + "--" + *nameIt + ((*it)->type != "" ? " <" + (*it)->type + ">" : "");
        }
        base += "\n";
        base += "    " + (*it)->description + "\n\n";
    }
    return base;
}

QStringList ScriptCommandLineParser::optionNames(ScriptCommandLineParser::Option *option) const{
    QStringList base;
    for ( QStringList::const_iterator nameIt = option->shortNames.begin(); nameIt != option->shortNames.end(); ++nameIt ){
        base << *nameIt;
    }
    for ( QStringList::const_iterator nameIt = option->longNames.begin(); nameIt != option->longNames.end(); ++nameIt ){
        base << *nameIt;
    }
    return base;
}


void ScriptCommandLineParser::assignName(const QString &name, ScriptCommandLineParser::Option *option, const QList<Option *> &check){
    if ( name.startsWith("--") ){
        if ( name.length() != 2 ){
            ScriptCommandLineParser::Option* sameNameOpt = findOptionByLongName(name.mid(2), check);
            if ( sameNameOpt != 0 )
                throw makeException("Option with the same name exists: " + name);
            option->longNames.append(name.mid(2));
        }
    } else if ( name.startsWith("-") ){
        if ( name.length() != 2 ){
            delete option;
            throw makeException("Long option name given for short option: " + name);
        }
        ScriptCommandLineParser::Option* sameNameOpt = findOptionByShortName(name.mid(1), check);
        if ( sameNameOpt != 0 )
            throw makeException("Option with the same name exists: " + name);
        option->shortNames.append(name.mid(1));
    } else
        throw makeException("Failed to parse option name: " + name);
}

ScriptCommandLineParser::Option *ScriptCommandLineParser::findOptionByLongName(const QString &name, const QList<Option *> &check){
    for ( QList<ScriptCommandLineParser::Option*>::const_iterator it = check.begin(); it != check.end(); ++it ){
        for ( QStringList::const_iterator nameIt = (*it)->longNames.begin(); nameIt != (*it)->longNames.end(); ++nameIt ){
            if ( *nameIt == name )
                return *it;
        }
    }
    return 0;
}

ScriptCommandLineParser::Option *ScriptCommandLineParser::findOptionByShortName(const QString &name, const QList<Option *> &check){
    for ( QList<ScriptCommandLineParser::Option*>::const_iterator it = check.begin(); it != check.end(); ++it ){
        for ( QStringList::const_iterator nameIt = (*it)->shortNames.begin(); nameIt != (*it)->shortNames.end(); ++nameIt ){
            if ( *nameIt == name )
                return *it;
        }
    }
    return 0;
}


void ScriptCommandLineParser::resetScriptOptions(){
    for ( QList<ScriptCommandLineParser::Option*>::iterator it = m_options.begin(); it != m_options.end(); ++it ){
        delete *it;
    }
    m_options.clear();
    m_arguments.clear();

    m_helpOption    = addFlag(QStringList() << "-h" << "--help",    "Displays this information and exits.");
    m_versionOption = addFlag(QStringList() << "-v" << "--version", "Displays version information and exits.");
}

ScriptCommandLineParser::Option *ScriptCommandLineParser::addFlag(const QStringList &names, const QString &description){
    if ( names.isEmpty() )
        throw makeException("Setting up option requires at least one name.");

    ScriptCommandLineParser::Option* option = new ScriptCommandLineParser::Option;
    for ( QStringList::const_iterator it = names.begin(); it != names.end(); ++it )
        assignName(*it, option, m_options);
    option->isFlag      = true;
    option->description = description;
    m_options.append(option);
    return option;
}

ScriptCommandLineParser::Option *ScriptCommandLineParser::addFlag(const QString &name, const QString &description){
    ScriptCommandLineParser::Option* option = new ScriptCommandLineParser::Option;
    assignName(name, option, m_options);
    option->isFlag      = true;
    option->description = description;
    m_options.append(option);
    return option;
}

ScriptCommandLineParser::Option *ScriptCommandLineParser::addOption(const QStringList &names, const QString &description, const QString &type){
    if ( names.isEmpty() )
        throw makeException("Setting up option requires at least one name.");

    ScriptCommandLineParser::Option* option = new ScriptCommandLineParser::Option;
    for ( QStringList::const_iterator it = names.begin(); it != names.end(); ++it )
        assignName(*it, option, m_options);
    option->isFlag      = false;
    option->description = description;
    option->type        = type;
    m_options.append(option);
    return option;
}

ScriptCommandLineParser::Option *ScriptCommandLineParser::addOption(const QString &name, const QString &description, const QString &type){
    ScriptCommandLineParser::Option* option = new ScriptCommandLineParser::Option;
    assignName(name, option, m_options);
    option->isFlag      = false;
    option->description = description;
    option->type        = type;
    m_options.append(option);
    return option;
}

void ScriptCommandLineParser::parseArguments(){
    if ( m_argvTail.length() > 0 ){
        int i = 0;
        while ( i < m_argvTail.length() ){
            if ( m_argvTail[i][0] == '-' ){
                if ( m_argvTail[i][1] == '-' ){
                    ScriptCommandLineParser::Option* option = findOptionByLongName(m_argvTail[i].mid(2), m_options);
                    if ( !option )
                        throw makeException(QString("Option not found: ") + m_argvTail[i]);

                    option->isSet = true;
                    if ( !option->isFlag ){
                        ++i;
                        if ( i == m_argvTail.length() )
                            throw makeException("No value given for option: " + QString(m_argvTail[i - 1]));
                        option->value = m_argvTail[i];
                    }
                } else {
                    ScriptCommandLineParser::Option* option = findOptionByShortName(m_argvTail[i].mid(1), m_options);
                    if ( !option )
                        throw makeException(QString("Option not found: ") + m_argvTail[i]);

                    option->isSet = true;
                    if ( !option->isFlag ){
                        ++i;
                        if ( i == m_argvTail.length() )
                            throw makeException("No value given for option: " + QString(m_argvTail[i - 1]));
                        option->value = m_argvTail[i];
                    }
                }
            } else {
                m_arguments.append(m_argvTail[i]);
                break;
            }
            ++i;
        }

        while ( i < m_argvTail.length() ){
            m_arguments.append(m_argvTail[i]);
            ++i;
        }
    }
}

ScriptCommandLineParser::Option* ScriptCommandLineParser::findOptionByName(const QString &name){
    if ( name.startsWith("--") )
        return findOptionByLongName(name.mid(2), m_options);
    else if ( name.startsWith("-") )
        return findOptionByShortName(name.mid(1), m_options);
    return 0;
}

bool ScriptCommandLineParser::isSet(ScriptCommandLineParser::Option *option) const{
    return option->isSet;
}

const QString &ScriptCommandLineParser::value(ScriptCommandLineParser::Option *option) const{
    return option->value;
}

void ScriptCommandLineParser::assertIsSet(ScriptCommandLineParser::Option *option) const{
    if ( !isSet(option) ){
        QString key = option->longNames.length() > 0 ? "--" + option->longNames.first() : "-" + option->shortNames.first();
        throw makeException("Required option has not been set: " + key);
    }
}

}// namespace
