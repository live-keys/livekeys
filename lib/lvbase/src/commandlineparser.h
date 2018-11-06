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

#ifndef LVCOMMANDLINEPARSER_H
#define LVCOMMANDLINEPARSER_H

#include "live/lvbaseglobal.h"
#include "live/exception.h"

#include <list>
#include <string>

namespace lv{

class LV_BASE_EXPORT CommandLineParserException : public Exception{
public:
    CommandLineParserException(const std::string& message, int code = 0): Exception(message, code){}
};

class CommandLineParserPrivate;
class LV_BASE_EXPORT CommandLineParser{

public:
    class Option;

public:
    CommandLineParser(const std::string& header);
    ~CommandLineParser();

    void parse(int argc, const char* const argv[]);

    Option* helpOption();
    Option* versionOption();

    Option* addFlag(const std::vector<std::string>& names, const std::string& description);
    Option* addOption(const std::vector<std::string>& names, const std::string& description, const std::string& type);

    const std::string &script() const;
    const std::vector<std::string>& scriptArguments() const;

    std::string helpString() const;
    std::vector<std::string> optionNames(Option* option) const;

    bool isSet(Option* option) const;
    const std::string& value(Option* option) const;
    void assertIsSet(Option* option) const;

private:
    DISABLE_COPY(CommandLineParser);

    void assignName(const std::string& name, Option* option, const std::vector<Option*>& check);
    Option* findOptionByLongName(const std::string& name, const std::vector<Option*>& check);
    Option* findOptionByShortName(const std::string& name, const std::vector<Option*>& check);

    Option*        m_helpOption;
    Option*        m_versionOption;

    CommandLineParserPrivate* m_d;
};

inline CommandLineParser::Option *CommandLineParser::helpOption(){
    return m_helpOption;
}

inline CommandLineParser::Option *CommandLineParser::versionOption(){
    return m_versionOption;
}


}// namespace

#endif // LVCOMMANDLINEPARSER_H
