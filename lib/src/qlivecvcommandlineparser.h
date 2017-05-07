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

#ifndef QCOMMANDLINEPARSER_H
#define QCOMMANDLINEPARSER_H

#include <QString>
#include <QList>
#include "qliveglobal.h"
#include "qlivecvexception.h"

namespace lcv{

class Q_LIVE_EXPORT QLiveCVCommandLineException : public QLiveCVException{
public:
    QLiveCVCommandLineException(const QString& message, int code = 0): QLiveCVException(message, code){}
};


class Q_LIVE_EXPORT QLiveCVCommandLineParser{

public:
    class Option;

public:
    QLiveCVCommandLineParser(const QString& header);
    ~QLiveCVCommandLineParser();

    void parse(int argc, const char* const argv[]);

    Option* helpOption();
    Option* versionOption();

    Option* addFlag(const QStringList& names, const QString& description);
    Option* addFlag(const QString& name, const QString& description);
    Option* addOption(const QStringList& names, const QString& description, const QString& type);
    Option* addOption(const QString& name, const QString& description, const QString& type);
    const QString &script() const;

    QString helpString() const;

    Option* scriptHelpOption();
    Option* scriptVersionOption();

    void resetScriptOptions();
    Option* addScriptFlag(const QStringList& names, const QString& description);
    Option* addScriptFlag(const QString& name, const QString& description);
    Option* addScriptOption(const QStringList& names, const QString& description, const QString& type);
    Option* addScriptOption(const QString& name, const QString& description, const QString& type);
    void parseScriptArguments();

    QString scriptHelpString() const;

    const QStringList& scriptArguments();
    const QStringList& scriptSentArguments();

    QStringList optionNames(Option* option) const;

    bool isSet(Option* option) const;
    const QString& value(Option* option) const;
    void assertIsSet(Option* option) const;

    Option *findScriptOptionByName(const QString& name);

private:
    void assignName(const QString& name, Option* option, const QList<Option*>& check);
    Option* findOptionByLongName(const QString& name, const QList<Option*>& check);
    Option* findOptionByShortName(const QString& name, const QList<Option*>& check);

    Option*        m_helpOption;
    Option*        m_versionOption;

    Option*        m_scriptHelpOption;
    Option*        m_scriptVersionOption;

    QList<Option*> m_options;
    QString        m_header;
    QString        m_version;

    QList<Option*> m_scriptOptions;
    QStringList    m_scriptArguments;
    QStringList    m_scriptSentArguments;
    QString        m_script;
};

inline QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::helpOption(){
    return m_helpOption;
}

inline QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::versionOption(){
    return m_versionOption;
}

inline const QString &QLiveCVCommandLineParser::script() const{
    return m_script;
}

inline const QStringList &QLiveCVCommandLineParser::scriptArguments(){
    return m_scriptArguments;
}

inline const QStringList &QLiveCVCommandLineParser::scriptSentArguments(){
    return m_scriptSentArguments;
}

inline QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::scriptHelpOption(){
    return m_scriptHelpOption;
}

inline QLiveCVCommandLineParser::Option *QLiveCVCommandLineParser::scriptVersionOption(){
    return m_scriptVersionOption;
}

}// namespace

#endif // QCOMMANDLINEPARSER_H
