/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
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

#ifndef SCRIPTCOMMANDLINEPARSER_H
#define SCRIPTCOMMANDLINEPARSER_H

#include <QString>
#include <QList>

namespace lv{

/// \private
class QmlScriptArgumentsParser{

public:
    class Option;

public:
    QmlScriptArgumentsParser(const QStringList& argvTail);
    ~QmlScriptArgumentsParser();

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

inline const QStringList &QmlScriptArgumentsParser::arguments(){
    return m_arguments;
}

inline QmlScriptArgumentsParser::Option *QmlScriptArgumentsParser::helpOption(){
    return m_helpOption;
}

inline QmlScriptArgumentsParser::Option *QmlScriptArgumentsParser::versionOption(){
    return m_versionOption;
}

}// namespace

#endif // SCRIPTCOMMANDLINEPARSER_H
