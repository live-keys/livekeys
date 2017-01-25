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

#include "qlivecvcommandlineparser.h"

namespace lcv{

class QLiveCVCommandLineParser::Option{

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


void QLiveCVCommandLineParser::assignName(const QString &name, QLiveCVCommandLineParser::Option *option, const QList<Option *> &check){
    if ( name.startsWith("--") ){
        if ( name.length() != 2 ){
            QLiveCVCommandLineParser::Option* sameNameOpt = findOptionByLongName(name.mid(2), check);
            if ( sameNameOpt != 0 )
                throw QLiveCVCommandLineException("Option with the same name exists: " + name);
            option->longNames.append(name.mid(2));
        }
    } else if ( name.startsWith("-") ){
        if ( name.length() != 2 ){
            delete option;
            throw QLiveCVCommandLineException("Long option name given for short option: " + name);
        }
        QLiveCVCommandLineParser::Option* sameNameOpt = findOptionByShortName(name.mid(1), check);
        if ( sameNameOpt != 0 )
            throw QLiveCVCommandLineException("Option with the same name exists: " + name);
        option->shortNames.append(name.mid(1));
    } else
        throw QLiveCVCommandLineException("Failed to parse option name: " + name);
}


QLiveCVCommandLineParser::QLiveCVCommandLineParser(const QString &header)
    : m_header(header)
{
    m_helpOption    = addFlag(QStringList() << "-h" << "--help",    "Displays this information and exits.");
    m_versionOption = addFlag(QStringList() << "-v" << "--version", "Displays version information and exits.");

    m_scriptHelpOption    = addScriptFlag(QStringList() << "-h" << "--help",    "Displays this information and exits.");
    m_scriptVersionOption = addScriptFlag(QStringList() << "-v" << "--version", "Displays version information and exits.");
}

QLiveCVCommandLineParser::~QLiveCVCommandLineParser(){
    for ( QList<QLiveCVCommandLineParser::Option*>::iterator it = m_options.begin(); it != m_options.end(); ++it ){
        delete *it;
    }
    m_options.clear();

    for ( QList<QLiveCVCommandLineParser::Option*>::iterator it = m_scriptOptions.begin(); it != m_scriptOptions.end(); ++it ){
        delete *it;
    }
    m_scriptOptions.clear();
}

QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::addFlag(const QStringList &names, const QString &description){
    if ( names.isEmpty() )
        throw QLiveCVCommandLineException("Setting up option requires at least one name.");

    QLiveCVCommandLineParser::Option* option = new QLiveCVCommandLineParser::Option;
    for ( QStringList::const_iterator it = names.begin(); it != names.end(); ++it )
        assignName(*it, option, m_options);
    option->isFlag      = true;
    option->description = description;
    m_options.append(option);
    return option;
}

QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::addFlag(const QString &name, const QString &description){
    QLiveCVCommandLineParser::Option* option = new QLiveCVCommandLineParser::Option;
    assignName(name, option, m_options);
    option->isFlag      = true;
    option->description = description;
    m_options.append(option);
    return option;
}

QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::addOption(const QStringList &names, const QString &description, const QString &type){
    if ( names.isEmpty() )
        throw QLiveCVCommandLineException("Setting up option requires at least one name.");

    QLiveCVCommandLineParser::Option* option = new QLiveCVCommandLineParser::Option;
    for ( QStringList::const_iterator it = names.begin(); it != names.end(); ++it )
        assignName(*it, option, m_options);
    option->isFlag      = false;
    option->description = description;
    option->type        = type;
    m_options.append(option);
    return option;
}

QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::addOption(const QString &name, const QString &description, const QString &type){
    QLiveCVCommandLineParser::Option* option = new QLiveCVCommandLineParser::Option;
    assignName(name, option, m_options);
    option->isFlag      = false;
    option->description = description;
    option->type        = type;
    m_options.append(option);
    return option;
}

QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::findOptionByLongName(const QString &name, const QList<Option *> &check){
    for ( QList<QLiveCVCommandLineParser::Option*>::const_iterator it = check.begin(); it != check.end(); ++it ){
        for ( QStringList::const_iterator nameIt = (*it)->longNames.begin(); nameIt != (*it)->longNames.end(); ++nameIt ){
            if ( *nameIt == name )
                return *it;
        }
    }
    return 0;
}

QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::findOptionByShortName(const QString &name, const QList<Option *> &check){
    for ( QList<QLiveCVCommandLineParser::Option*>::const_iterator it = check.begin(); it != check.end(); ++it ){
        for ( QStringList::const_iterator nameIt = (*it)->shortNames.begin(); nameIt != (*it)->shortNames.end(); ++nameIt ){
            if ( *nameIt == name )
                return *it;
        }
    }
    return 0;
}

QString QLiveCVCommandLineParser::helpString() const{
    QString base("\n" + m_header + "\n\n" + "Usage:\n\n   livecv [options...] script.qml [args ...]\n\nOptions:\n\n");
    for ( QList<QLiveCVCommandLineParser::Option*>::const_iterator it = m_options.begin(); it != m_options.end(); ++it ){
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

void QLiveCVCommandLineParser::resetScriptOptions(){
    for ( QList<QLiveCVCommandLineParser::Option*>::iterator it = m_scriptOptions.begin(); it != m_scriptOptions.end(); ++it ){
        delete *it;
    }
    m_scriptOptions.clear();
    m_scriptArguments.clear();

    m_scriptHelpOption    = addScriptFlag(QStringList() << "-h" << "--help",    "Displays this information and exits.");
    m_scriptVersionOption = addScriptFlag(QStringList() << "-v" << "--version", "Displays version information and exits.");
}

QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::addScriptFlag(const QStringList &names, const QString &description){
    if ( names.isEmpty() )
        throw QLiveCVCommandLineException("Setting up option requires at least one name.");

    QLiveCVCommandLineParser::Option* option = new QLiveCVCommandLineParser::Option;
    for ( QStringList::const_iterator it = names.begin(); it != names.end(); ++it )
        assignName(*it, option, m_scriptOptions);
    option->isFlag      = true;
    option->description = description;
    m_scriptOptions.append(option);
    return option;
}

QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::addScriptFlag(const QString &name, const QString &description){
    QLiveCVCommandLineParser::Option* option = new QLiveCVCommandLineParser::Option;
    assignName(name, option, m_scriptOptions);
    option->isFlag      = true;
    option->description = description;
    m_scriptOptions.append(option);
    return option;
}

QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::addScriptOption(const QStringList &names, const QString &description, const QString &type){
    if ( names.isEmpty() )
        throw QLiveCVCommandLineException("Setting up option requires at least one name.");

    QLiveCVCommandLineParser::Option* option = new QLiveCVCommandLineParser::Option;
    for ( QStringList::const_iterator it = names.begin(); it != names.end(); ++it )
        assignName(*it, option, m_scriptOptions);
    option->isFlag      = false;
    option->description = description;
    option->type        = type;
    m_scriptOptions.append(option);
    return option;
}

QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::addScriptOption(const QString &name, const QString &description, const QString &type){
    QLiveCVCommandLineParser::Option* option = new QLiveCVCommandLineParser::Option;
    assignName(name, option, m_scriptOptions);
    option->isFlag      = false;
    option->description = description;
    option->type        = type;
    m_scriptOptions.append(option);
    return option;
}

void QLiveCVCommandLineParser::parseScriptArguments(){
    if ( m_scriptSentArguments.length() > 1 ){
        int i = 1;
        while ( i < m_scriptSentArguments.length() ){
            if ( m_scriptSentArguments[i][0] == '-' ){
                if ( m_scriptSentArguments[i][1] == '-' ){
                    QLiveCVCommandLineParser::Option* option = findOptionByLongName(m_scriptSentArguments[i].mid(2), m_scriptOptions);
                    if ( !option )
                        throw QLiveCVCommandLineException(QString("Option not found: ") + m_scriptSentArguments[i]);

                    option->isSet = true;
                    if ( !option->isFlag ){
                        ++i;
                        if ( i == m_scriptSentArguments.length() )
                            throw QLiveCVCommandLineException("No value given for option: " + QString(m_scriptSentArguments[i - 1]));
                        option->value = m_scriptSentArguments[i];
                    }
                } else {
                    QLiveCVCommandLineParser::Option* option = findOptionByShortName(m_scriptSentArguments[i].mid(1), m_scriptOptions);
                    if ( !option )
                        throw QLiveCVCommandLineException(QString("Option not found: ") + m_scriptSentArguments[i]);

                    option->isSet = true;
                    if ( !option->isFlag ){
                        ++i;
                        if ( i == m_scriptSentArguments.length() )
                            throw QLiveCVCommandLineException("No value given for option: " + QString(m_scriptSentArguments[i - 1]));
                        option->value = m_scriptSentArguments[i];
                    }
                }
            } else {
                m_scriptArguments.append(m_scriptSentArguments[i]);
                break;
            }
            ++i;
        }

        while ( i < m_scriptSentArguments.length() ){
            m_scriptArguments.append(m_scriptSentArguments[i]);
            ++i;
        }
    }
}

QString QLiveCVCommandLineParser::scriptHelpString() const{
    QString base("\n" + m_header + "\n\n" + "Usage:\n\n   livecv [...] script.qml [options] [args ...]\n\nScript options:\n\n");
    for ( QList<QLiveCVCommandLineParser::Option*>::const_iterator it = m_scriptOptions.begin(); it != m_scriptOptions.end(); ++it ){
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

QStringList QLiveCVCommandLineParser::optionNames(QLiveCVCommandLineParser::Option *option) const{
    QStringList base;
    for ( QStringList::const_iterator nameIt = option->shortNames.begin(); nameIt != option->shortNames.end(); ++nameIt ){
        base << *nameIt;
    }
    for ( QStringList::const_iterator nameIt = option->longNames.begin(); nameIt != option->longNames.end(); ++nameIt ){
        base << *nameIt;
    }
    return base;
}

bool QLiveCVCommandLineParser::isSet(QLiveCVCommandLineParser::Option *option) const{
    return option->isSet;
}

const QString &QLiveCVCommandLineParser::value(QLiveCVCommandLineParser::Option *option) const{
    return option->value;
}

void QLiveCVCommandLineParser::assertIsSet(QLiveCVCommandLineParser::Option *option) const{
    if ( !isSet(option) ){
        QString key = option->longNames.length() > 0 ? "--" + option->longNames.first() : "-" + option->shortNames.first();
        throw QLiveCVCommandLineException("Required option has not been set: " + key);
    }
}

QLiveCVCommandLineParser::Option* QLiveCVCommandLineParser::findScriptOptionByName(const QString &name){
    if ( name.startsWith("--") )
        return findOptionByLongName(name.mid(2), m_scriptOptions);
    else if ( name.startsWith("-") )
        return findOptionByShortName(name.mid(1), m_scriptOptions);
    return 0;
}

void QLiveCVCommandLineParser::parse(int argc, const char * const argv[]){
    if ( argc > 1 ){
        int i = 1;
        while ( i < argc ){
            if ( argv[i][0] == '-' ){
                if ( argv[i][1] == '-' ){
                    QLiveCVCommandLineParser::Option* option = findOptionByLongName(QString(&argv[i][2]), m_options);
                    if ( !option )
                        throw QLiveCVCommandLineException(QString("Option not found: ") + argv[i]);

                    if ( option->isFlag )
                        option->isSet = true;
                    else {
                        ++i;
                        if ( i == argc )
                            throw QLiveCVCommandLineException("No value given for option: " + QString(argv[i - 1]));
                        option->isSet = true;
                        option->value = argv[i];
                    }
                } else {
                    QLiveCVCommandLineParser::Option* option = findOptionByShortName(QString(&argv[i][1]), m_options);
                    if ( !option )
                        throw QLiveCVCommandLineException(QString("Option not found: ") + argv[i]);

                    if ( option->isFlag )
                        option->isSet = true;
                    else {
                        ++i;
                        if ( i == argc )
                            throw QLiveCVCommandLineException("No value given for option: " + QString(argv[i - 1]));
                        option->isSet = true;
                        option->value = argv[i];
                    }
                }
            } else {
                m_script = argv[i];
                break;
            }
            ++i;
        }

        while ( i < argc ){
            m_scriptSentArguments.append(argv[i]);
            ++i;
        }
    }
}

}// namespace
