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

#include "live/commandlineparser.h"

namespace lv{

class CommandLineParser::Option{

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


void CommandLineParser::assignName(const QString &name, CommandLineParser::Option *option, const QList<Option *> &check){
    if ( name.startsWith("--") ){
        if ( name.length() != 2 ){
            CommandLineParser::Option* sameNameOpt = findOptionByLongName(name.mid(2), check);
            if ( sameNameOpt != 0 )
                throw CommandLineParserException("Option with the same name exists: " + name);
            option->longNames.append(name.mid(2));
        }
    } else if ( name.startsWith("-") ){
        if ( name.length() != 2 ){
            delete option;
            throw CommandLineParserException("Long option name given for short option: " + name);
        }
        CommandLineParser::Option* sameNameOpt = findOptionByShortName(name.mid(1), check);
        if ( sameNameOpt != 0 )
            throw CommandLineParserException("Option with the same name exists: " + name);
        option->shortNames.append(name.mid(1));
    } else
        throw CommandLineParserException("Failed to parse option name: " + name);
}


CommandLineParser::CommandLineParser(const QString &header)
    : m_header(header)
{
    m_helpOption    = addFlag(QStringList() << "-h" << "--help",    "Displays this information and exits.");
    m_versionOption = addFlag(QStringList() << "-v" << "--version", "Displays version information and exits.");
}

CommandLineParser::~CommandLineParser(){
    for ( QList<CommandLineParser::Option*>::iterator it = m_options.begin(); it != m_options.end(); ++it ){
        delete *it;
    }
    m_options.clear();
}

CommandLineParser::Option *CommandLineParser::addFlag(const QStringList &names, const QString &description){
    if ( names.isEmpty() )
        throw CommandLineParserException("Setting up option requires at least one name.");

    CommandLineParser::Option* option = new CommandLineParser::Option;
    for ( QStringList::const_iterator it = names.begin(); it != names.end(); ++it )
        assignName(*it, option, m_options);
    option->isFlag      = true;
    option->description = description;
    m_options.append(option);
    return option;
}

CommandLineParser::Option *CommandLineParser::addFlag(const QString &name, const QString &description){
    CommandLineParser::Option* option = new CommandLineParser::Option;
    assignName(name, option, m_options);
    option->isFlag      = true;
    option->description = description;
    m_options.append(option);
    return option;
}

CommandLineParser::Option *CommandLineParser::addOption(const QStringList &names, const QString &description, const QString &type){
    if ( names.isEmpty() )
        throw CommandLineParserException("Setting up option requires at least one name.");

    CommandLineParser::Option* option = new CommandLineParser::Option;
    for ( QStringList::const_iterator it = names.begin(); it != names.end(); ++it )
        assignName(*it, option, m_options);
    option->isFlag      = false;
    option->description = description;
    option->type        = type;
    m_options.append(option);
    return option;
}

CommandLineParser::Option *CommandLineParser::addOption(const QString &name, const QString &description, const QString &type){
    CommandLineParser::Option* option = new CommandLineParser::Option;
    assignName(name, option, m_options);
    option->isFlag      = false;
    option->description = description;
    option->type        = type;
    m_options.append(option);
    return option;
}

CommandLineParser::Option *CommandLineParser::findOptionByLongName(const QString &name, const QList<Option *> &check){
    for ( QList<CommandLineParser::Option*>::const_iterator it = check.begin(); it != check.end(); ++it ){
        for ( QStringList::const_iterator nameIt = (*it)->longNames.begin(); nameIt != (*it)->longNames.end(); ++nameIt ){
            if ( *nameIt == name )
                return *it;
        }
    }
    return 0;
}

CommandLineParser::Option *CommandLineParser::findOptionByShortName(const QString &name, const QList<Option *> &check){
    for ( QList<CommandLineParser::Option*>::const_iterator it = check.begin(); it != check.end(); ++it ){
        for ( QStringList::const_iterator nameIt = (*it)->shortNames.begin(); nameIt != (*it)->shortNames.end(); ++nameIt ){
            if ( *nameIt == name )
                return *it;
        }
    }
    return 0;
}

QString CommandLineParser::helpString() const{
    QString base("\n" + m_header + "\n\n" + "Usage:\n\n   livecv [options...] script.qml [args ...]\n\nOptions:\n\n");
    for ( QList<CommandLineParser::Option*>::const_iterator it = m_options.begin(); it != m_options.end(); ++it ){
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

QStringList CommandLineParser::optionNames(CommandLineParser::Option *option) const{
    QStringList base;
    for ( QStringList::const_iterator nameIt = option->shortNames.begin(); nameIt != option->shortNames.end(); ++nameIt ){
        base << *nameIt;
    }
    for ( QStringList::const_iterator nameIt = option->longNames.begin(); nameIt != option->longNames.end(); ++nameIt ){
        base << *nameIt;
    }
    return base;
}


bool CommandLineParser::isSet(CommandLineParser::Option *option) const{
    return option->isSet;
}

const QString &CommandLineParser::value(CommandLineParser::Option *option) const{
    return option->value;
}

void CommandLineParser::assertIsSet(CommandLineParser::Option *option) const{
    if ( !isSet(option) ){
        QString key = option->longNames.length() > 0 ? "--" + option->longNames.first() : "-" + option->shortNames.first();
        throw CommandLineParserException("Required option has not been set: " + key);
    }
}


void CommandLineParser::parse(int argc, const char * const argv[]){
    if ( argc > 1 ){
        int i = 1;
        while ( i < argc ){
            if ( argv[i][0] == '-' ){
                if ( argv[i][1] == '-' ){
                    CommandLineParser::Option* option = findOptionByLongName(QString(&argv[i][2]), m_options);
                    if ( !option )
                        throw CommandLineParserException(QString("Option not found: ") + argv[i]);

                    if ( option->isFlag )
                        option->isSet = true;
                    else {
                        ++i;
                        if ( i == argc )
                            throw CommandLineParserException("No value given for option: " + QString(argv[i - 1]));
                        option->isSet = true;
                        option->value = argv[i];
                    }
                } else {
                    CommandLineParser::Option* option = findOptionByShortName(QString(&argv[i][1]), m_options);
                    if ( !option )
                        throw CommandLineParserException(QString("Option not found: ") + argv[i]);

                    if ( option->isFlag )
                        option->isSet = true;
                    else {
                        ++i;
                        if ( i == argc )
                            throw CommandLineParserException("No value given for option: " + QString(argv[i - 1]));
                        option->isSet = true;
                        option->value = argv[i];
                    }
                }
            } else {
                m_script = argv[i];
                ++i;
                break;
            }
            ++i;
        }

        while ( i < argc ){
            m_scriptArguments.append(argv[i]);
            ++i;
        }
    }
}

}// namespace
