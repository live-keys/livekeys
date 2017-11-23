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

#ifndef LVCOMMANDLINEPARSER_H
#define LVCOMMANDLINEPARSER_H

#include <QString>
#include <QList>
#include "live/exception.h"

namespace lv{

class LV_BASE_EXPORT CommandLineParserException : public Exception{
public:
    CommandLineParserException(const QString& message, int code = 0): Exception(message, code){}
};


class LV_BASE_EXPORT CommandLineParser{

public:
    class Option;

public:
    CommandLineParser(const QString& header);
    ~CommandLineParser();

    void parse(int argc, const char* const argv[]);

    Option* helpOption();
    Option* versionOption();

    Option* addFlag(const QStringList& names, const QString& description);
    Option* addFlag(const QString& name, const QString& description);
    Option* addOption(const QStringList& names, const QString& description, const QString& type);
    Option* addOption(const QString& name, const QString& description, const QString& type);

    const QString &script() const;
    const QStringList& scriptArguments() const;

    QString helpString() const;
    QStringList optionNames(Option* option) const;

    bool isSet(Option* option) const;
    const QString& value(Option* option) const;
    void assertIsSet(Option* option) const;


private:
    void assignName(const QString& name, Option* option, const QList<Option*>& check);
    Option* findOptionByLongName(const QString& name, const QList<Option*>& check);
    Option* findOptionByShortName(const QString& name, const QList<Option*>& check);

    Option*        m_helpOption;
    Option*        m_versionOption;

    QList<Option*> m_options;
    QString        m_header;
    QString        m_version;

    QString        m_script;
    QStringList    m_scriptArguments;
};

inline CommandLineParser::Option *CommandLineParser::helpOption(){
    return m_helpOption;
}

inline CommandLineParser::Option *CommandLineParser::versionOption(){
    return m_versionOption;
}

inline const QString &CommandLineParser::script() const{
    return m_script;
}

inline const QStringList &CommandLineParser::scriptArguments() const{
    return m_scriptArguments;
}


}// namespace

#endif // LVCOMMANDLINEPARSER_H
