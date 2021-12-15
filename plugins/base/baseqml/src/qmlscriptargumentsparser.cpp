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

#include "qmlscriptargumentsparser.h"
#include "live/exception.h"

namespace{

    /// \private
    std::exception makeException(const QString& str){
        return CREATE_EXCEPTION(lv::Exception, str.toStdString().c_str(), lv::Exception::toCode("QmlScriptArgumentsParser"));
    }

}// namespace

namespace lv{

/// \private
class QmlScriptArgumentsParser::Option{

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

QmlScriptArgumentsParser::QmlScriptArgumentsParser(const QStringList &argvTail)
    : m_argvTail(argvTail)
{
    m_helpOption    = addFlag(QStringList() << "-h" << "--help",    "Displays this information and exits.");
    m_versionOption = addFlag(QStringList() << "-v" << "--version", "Displays version information and exits.");
}

QmlScriptArgumentsParser::~QmlScriptArgumentsParser(){
    for ( QList<QmlScriptArgumentsParser::Option*>::iterator it = m_options.begin(); it != m_options.end(); ++it ){
        delete *it;
    }
    m_options.clear();
}

QString QmlScriptArgumentsParser::helpString() const{
    QString base("\nUsage:\n\n   livekeys [...] script.qml [options] [args ...]\n\nScript options:\n\n");
    for ( QList<QmlScriptArgumentsParser::Option*>::const_iterator it = m_options.begin(); it != m_options.end(); ++it ){
        for ( auto nameIt = (*it)->shortNames.begin(); nameIt != (*it)->shortNames.end(); ++nameIt ){
            base += QString("  ") + "-" + *nameIt + ((*it)->type != "" ? " <" + (*it)->type + ">" : "");
        }
        for ( auto nameIt = (*it)->longNames.begin(); nameIt != (*it)->longNames.end(); ++nameIt ){
            base += QString("  ") + "--" + *nameIt + ((*it)->type != "" ? " <" + (*it)->type + ">" : "");
        }
        base += "\n";
        base += "    " + (*it)->description + "\n\n";
    }
    return base;
}

QStringList QmlScriptArgumentsParser::optionNames(QmlScriptArgumentsParser::Option *option) const{
    QStringList base;
    for ( QStringList::const_iterator nameIt = option->shortNames.begin(); nameIt != option->shortNames.end(); ++nameIt ){
        base << *nameIt;
    }
    for ( QStringList::const_iterator nameIt = option->longNames.begin(); nameIt != option->longNames.end(); ++nameIt ){
        base << *nameIt;
    }
    return base;
}


void QmlScriptArgumentsParser::assignName(const QString &name, QmlScriptArgumentsParser::Option *option, const QList<Option *> &check){
    if ( name.startsWith("--") ){
        if ( name.length() != 2 ){
            QmlScriptArgumentsParser::Option* sameNameOpt = findOptionByLongName(name.mid(2), check);
            if ( sameNameOpt != 0 )
                throw makeException("Option with the same name exists: " + name);
            option->longNames.append(name.mid(2));
        }
    } else if ( name.startsWith("-") ){
        if ( name.length() != 2 ){
            delete option;
            throw makeException("Long option name given for short option: " + name);
        }
        QmlScriptArgumentsParser::Option* sameNameOpt = findOptionByShortName(name.mid(1), check);
        if ( sameNameOpt != 0 )
            throw makeException("Option with the same name exists: " + name);
        option->shortNames.append(name.mid(1));
    } else
        throw makeException("Failed to parse option name: " + name);
}

QmlScriptArgumentsParser::Option *QmlScriptArgumentsParser::findOptionByLongName(const QString &name, const QList<Option *> &check){
    for ( QList<QmlScriptArgumentsParser::Option*>::const_iterator it = check.begin(); it != check.end(); ++it ){
        for ( QStringList::const_iterator nameIt = (*it)->longNames.begin(); nameIt != (*it)->longNames.end(); ++nameIt ){
            if ( *nameIt == name )
                return *it;
        }
    }
    return 0;
}

QmlScriptArgumentsParser::Option *QmlScriptArgumentsParser::findOptionByShortName(const QString &name, const QList<Option *> &check){
    for ( QList<QmlScriptArgumentsParser::Option*>::const_iterator it = check.begin(); it != check.end(); ++it ){
        for ( QStringList::const_iterator nameIt = (*it)->shortNames.begin(); nameIt != (*it)->shortNames.end(); ++nameIt ){
            if ( *nameIt == name )
                return *it;
        }
    }
    return 0;
}


void QmlScriptArgumentsParser::resetScriptOptions(){
    for ( QList<QmlScriptArgumentsParser::Option*>::iterator it = m_options.begin(); it != m_options.end(); ++it ){
        delete *it;
    }
    m_options.clear();
    m_arguments.clear();

    m_helpOption    = addFlag(QStringList() << "-h" << "--help",    "Displays this information and exits.");
    m_versionOption = addFlag(QStringList() << "-v" << "--version", "Displays version information and exits.");
}

QmlScriptArgumentsParser::Option *QmlScriptArgumentsParser::addFlag(const QStringList &names, const QString &description){
    if ( names.isEmpty() )
        throw makeException("Setting up option requires at least one name.");

    QmlScriptArgumentsParser::Option* option = new QmlScriptArgumentsParser::Option;
    for ( QStringList::const_iterator it = names.begin(); it != names.end(); ++it )
        assignName(*it, option, m_options);
    option->isFlag      = true;
    option->description = description;
    m_options.append(option);
    return option;
}

QmlScriptArgumentsParser::Option *QmlScriptArgumentsParser::addFlag(const QString &name, const QString &description){
    QmlScriptArgumentsParser::Option* option = new QmlScriptArgumentsParser::Option;
    assignName(name, option, m_options);
    option->isFlag      = true;
    option->description = description;
    m_options.append(option);
    return option;
}

QmlScriptArgumentsParser::Option *QmlScriptArgumentsParser::addOption(const QStringList &names, const QString &description, const QString &type){
    if ( names.isEmpty() )
        throw makeException("Setting up option requires at least one name.");

    QmlScriptArgumentsParser::Option* option = new QmlScriptArgumentsParser::Option;
    for ( QStringList::const_iterator it = names.begin(); it != names.end(); ++it )
        assignName(*it, option, m_options);
    option->isFlag      = false;
    option->description = description;
    option->type        = type;
    m_options.append(option);
    return option;
}

QmlScriptArgumentsParser::Option *QmlScriptArgumentsParser::addOption(const QString &name, const QString &description, const QString &type){
    QmlScriptArgumentsParser::Option* option = new QmlScriptArgumentsParser::Option;
    assignName(name, option, m_options);
    option->isFlag      = false;
    option->description = description;
    option->type        = type;
    m_options.append(option);
    return option;
}

void QmlScriptArgumentsParser::parseArguments(){
    if ( m_argvTail.length() > 0 ){
        int i = 0;
        while ( i < m_argvTail.length() ){
            if ( m_argvTail[i][0] == '-' ){
                if ( m_argvTail[i][1] == '-' ){
                    QmlScriptArgumentsParser::Option* option = findOptionByLongName(m_argvTail[i].mid(2), m_options);
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
                    QmlScriptArgumentsParser::Option* option = findOptionByShortName(m_argvTail[i].mid(1), m_options);
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

QmlScriptArgumentsParser::Option* QmlScriptArgumentsParser::findOptionByName(const QString &name){
    if ( name.startsWith("--") )
        return findOptionByLongName(name.mid(2), m_options);
    else if ( name.startsWith("-") )
        return findOptionByShortName(name.mid(1), m_options);
    return 0;
}

bool QmlScriptArgumentsParser::isSet(QmlScriptArgumentsParser::Option *option) const{
    return option->isSet;
}

const QString &QmlScriptArgumentsParser::value(QmlScriptArgumentsParser::Option *option) const{
    return option->value;
}

void QmlScriptArgumentsParser::assertIsSet(QmlScriptArgumentsParser::Option *option) const{
    if ( !isSet(option) ){
        QString key = option->longNames.length() > 0 ? "--" + option->longNames.first() : "-" + option->shortNames.first();
        throw makeException("Required option has not been set: " + key);
    }
}

}// namespace
