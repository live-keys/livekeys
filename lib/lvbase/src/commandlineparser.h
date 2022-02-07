/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#ifndef LVCOMMANDLINEPARSER_H
#define LVCOMMANDLINEPARSER_H

#include "live/lvbaseglobal.h"
#include "live/exception.h"

#include <list>
#include <string>

namespace lv{

/**
 * \class lv::CommandLineParserException
 * \brief A simple exception class for the command line parser, with message and code params
 * \ingroup lvbase
 */
class LV_BASE_EXPORT CommandLineParserException : public Exception{
public:
    /**
     * \brief Default constructor for this exception
     */
    CommandLineParserException(const std::string& message, Exception::Code code = 0): Exception(message, code){}
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

    const std::vector<std::string>& arguments() const;

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

/**
 * \brief Returns the help option object containing flag names and description.
 */
inline CommandLineParser::Option *CommandLineParser::helpOption(){
    return m_helpOption;
}

/**
 * \brief Returns the version option object, containing flag names and description.
 */
inline CommandLineParser::Option *CommandLineParser::versionOption(){
    return m_versionOption;
}


}// namespace

#endif // LVCOMMANDLINEPARSER_H
