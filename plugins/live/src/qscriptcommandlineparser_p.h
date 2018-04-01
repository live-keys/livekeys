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

#ifndef QSCRIPTCOMMANDLINEPARSER_H
#define QSCRIPTCOMMANDLINEPARSER_H

#include <QString>
#include <QList>

class QScriptCommandLineParser{

public:
    class Option;

public:
    QScriptCommandLineParser(const QStringList& argvTail);
    ~QScriptCommandLineParser();

    Option* helpOption();
    Option* versionOption();

    void resetScriptOptions();
    Option* addFlag(const QStringList& names, const QString& description);
    Option* addFlag(const QString& name, const QString& description);
    Option* addOption(const QStringList& names, const QString& description, const QString& type);
    Option* addOption(const QString& name, const QString& description, const QString& type);
    void parseArguments();

    Option *findOptionByName(const QString& name);
    QString helpString() const;

    QStringList optionNames(Option* option) const;
    const QStringList& arguments();

    bool isSet(Option* option) const;
    const QString& value(Option* option) const;
    void assertIsSet(Option* option) const;

private:
    void assignName(const QString& name, Option* option, const QList<Option*>& check);
    Option* findOptionByLongName(const QString& name, const QList<Option*>& check);
    Option* findOptionByShortName(const QString& name, const QList<Option*>& check);

    QList<Option*> m_options;
    QStringList    m_arguments;
    QStringList    m_argvTail;

    Option*        m_helpOption;
    Option*        m_versionOption;
};

inline const QStringList &QScriptCommandLineParser::arguments(){
    return m_arguments;
}

inline QScriptCommandLineParser::Option *QScriptCommandLineParser::helpOption(){
    return m_helpOption;
}

inline QScriptCommandLineParser::Option *QScriptCommandLineParser::versionOption(){
    return m_versionOption;
}


#endif // QSCRIPTCOMMANDLINEPARSER_H
