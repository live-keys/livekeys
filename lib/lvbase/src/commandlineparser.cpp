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

#include "live/commandlineparser.h"
#include <sstream>

namespace lv{


/**
  \class lv::CommandLineParser
  \ingroup lvbase

  \brief Command line parser class.
  */

/// \private
class CommandLineParserPrivate{
public:
    std::vector<CommandLineParser::Option*> options;
    std::string              header;
    std::string              version;

    std::string              script;
    std::vector<std::string> scriptArguments;
};

class CommandLineParser::Option{

public:
    std::vector<std::string> shortNames;
    std::vector<std::string> longNames;
    std::string              description;
    std::string              type;
    bool                     isFlag;

    bool                     isSet;
    std::string              value;

    Option() : isFlag(false), isSet(false){}

    static bool nameStartsWith(const std::string& name, const std::string& pref);
};


bool CommandLineParser::Option::nameStartsWith(const std::string& name, const std::string &pref){
    if ( name.length() >= pref.length() ){
        if ( name.substr(0, pref.length()) == pref )
            return true;
    }
    return false;
}

void CommandLineParser::assignName(const std::string &name, CommandLineParser::Option *option, const std::vector<Option *> &check){
    if ( CommandLineParser::Option::nameStartsWith(name, "--") ){
        if ( name.length() != 2 ){
            CommandLineParser::Option* sameNameOpt = findOptionByLongName(name.substr(2), check);
            if ( sameNameOpt != 0 ){
                THROW_EXCEPTION(CommandLineParserException, "Option with the same name exists: " + name, 1);
            }
            option->longNames.push_back(name.substr(2));
        }
    } else if ( CommandLineParser::Option::nameStartsWith(name, "-") ){
        if ( name.length() != 2 ){
            delete option;
            THROW_EXCEPTION(CommandLineParserException, "Long option name given for short option: " + name, 2);
        }
        CommandLineParser::Option* sameNameOpt = findOptionByShortName(name.substr(1), check);
        if ( sameNameOpt != 0 )
            THROW_EXCEPTION(CommandLineParserException, "Option with the same name exists: " + name, 3);
        option->shortNames.push_back(name.substr(1));
    } else
        THROW_EXCEPTION(CommandLineParserException, "Failed to parse option name: " + name, 4);
}

/**
 * \brief Simple constructor with a single header parameter
 *
 * Header serves as a sort of title when displaying usage instructions.
 * This constructor also adds the two most common command line flags, "help" and "version".
 */
CommandLineParser::CommandLineParser(const std::string &header)
    : m_d(new CommandLineParserPrivate)
{
    m_d->header     = header;
    m_helpOption    = addFlag({"-h", "--help"},    "Displays this information and exits.");
    m_versionOption = addFlag({"-v", "--version"}, "Displays version information and exits.");
}

/**
 * \brief Destructor of CommandLineParser
 *
 * Erases all the pre-set options.
 */
CommandLineParser::~CommandLineParser(){
    for ( auto it = m_d->options.begin(); it != m_d->options.end(); ++it ){
        delete *it;
    }
    delete m_d;
}

/**
 * \brief Adds a flag with given name(s) and description.
 *
 * A flag is a special, bool-like kind of option where just stating it sets it to true.
 * A flag can have a short and/or long name(s), which has to be unique. If not, an exception is thrown.
 */
CommandLineParser::Option *CommandLineParser::addFlag(const std::vector<std::string> &names, const std::string &description){
    if ( names.empty() )
        THROW_EXCEPTION(CommandLineParserException, "Setting up option requires at least one name.", 1);

    CommandLineParser::Option* option = new CommandLineParser::Option;
    for ( auto it = names.begin(); it != names.end(); ++it )
        assignName(*it, option, m_d->options);
    option->isFlag      = true;
    option->description = description;
    m_d->options.push_back(option);
    return option;
}

/**
 * \brief Adds an option with given name(s), description and type.
 *
 * An option can have a short and/or long name(s), indicated by one or two dashes respectively.
 * The name(s) must be unique - if not, an exception is thrown. The third parameter serves as a
 * type descriptor. Flags are inherently bools, while options in general can be anything like strings,
 * paths, lists...
 */
CommandLineParser::Option *CommandLineParser::addOption(const std::vector<std::string> &names, const std::string &description, const std::string &type){
    if ( names.empty() )
        THROW_EXCEPTION(CommandLineParserException, "Setting up option requires at least one name.", 5);

    CommandLineParser::Option* option = new CommandLineParser::Option;
    for ( auto it = names.begin(); it != names.end(); ++it )
        assignName(*it, option, m_d->options);
    option->isFlag      = false;
    option->description = description;
    option->type        = type;
    m_d->options.push_back(option);
    return option;
}

CommandLineParser::Option *CommandLineParser::findOptionByLongName(const std::string &name, const std::vector<Option *> &check){
    for ( auto it = check.begin(); it != check.end(); ++it ){
        for ( auto nameIt = (*it)->longNames.begin(); nameIt != (*it)->longNames.end(); ++nameIt ){
            if ( *nameIt == name )
                return *it;
        }
    }
    return 0;
}

CommandLineParser::Option *CommandLineParser::findOptionByShortName(const std::string &name, const std::vector<Option *> &check){
    for ( auto it = check.begin(); it != check.end(); ++it ){
        for ( auto nameIt = (*it)->shortNames.begin(); nameIt != (*it)->shortNames.end(); ++nameIt ){
            if ( *nameIt == name )
                return *it;
        }
    }
    return 0;
}

/**
 * \brief Represents a string containing all of the pre-set options, their names, descriptions, types etc.
 *
 * A simple user instruction on how to pass arguments through the command line.
 */
std::string CommandLineParser::helpString() const{
    std::stringstream base;
    base << "\n" << m_d->header << "\n\n" << "Usage:\n\n   livecv [options...] script.qml [args ...]\n\nOptions:\n\n";
    for ( auto it = m_d->options.begin(); it != m_d->options.end(); ++it ){
        for ( auto nameIt = (*it)->shortNames.begin(); nameIt != (*it)->shortNames.end(); ++nameIt ){
            base << std::string("  ") << "-" << *nameIt << ((*it)->type != "" ? " <" + (*it)->type + ">" : "");
        }
        for ( auto nameIt = (*it)->longNames.begin(); nameIt != (*it)->longNames.end(); ++nameIt ){
            base << std::string("  ") << "--" << *nameIt << ((*it)->type != "" ? " <" + (*it)->type + ">" : "");
        }
        base << "\n";
        base << "    " + (*it)->description + "\n\n";
    }
    return base.str();

}

/**
 * \brief Returns the name(s) of a given option.
 *
 * Each option/flag must have at least one name, be it long or short. This function returns all of them.
 */
std::vector<std::string> CommandLineParser::optionNames(CommandLineParser::Option *option) const{
    std::vector<std::string> base;
    for ( auto nameIt = option->shortNames.begin(); nameIt != option->shortNames.end(); ++nameIt ){
        base.push_back(*nameIt);
    }
    for ( auto nameIt = option->longNames.begin(); nameIt != option->longNames.end(); ++nameIt ){
        base.push_back(*nameIt);
    }
    return base;
}

/**
 * \brief Returns an indicator that the option is set in the parsed batch of arguments.
 *
 * i.e. if we include "--version" as an argument, the isSet value of that flag is going to be true.
 */
bool CommandLineParser::isSet(CommandLineParser::Option *option) const{
    return option->isSet;
}


/**
 * \brief Returns the parsed value for the given option.
 *
 * Each non-flag option has another argument following it. This is stored as the option's value,
 * and returned by this function.
 */
const std::string &CommandLineParser::value(CommandLineParser::Option *option) const{
    return option->value;
}

/**
 * \brief Asserts that the provided option is set in the parsed arguments.
 *
 * If not, CommandLineParserException is thrown.
 */
void CommandLineParser::assertIsSet(CommandLineParser::Option *option) const{
    if ( !isSet(option) ){
        std::string key = option->longNames.size() > 0 ? "--" + option->longNames.front() : "-" + option->shortNames.front();
        THROW_EXCEPTION(CommandLineParserException, "Required option has not been set: " + key, 1);
    }
}

/**
 * \brief Main function of this class, parses the given number of strings
 *
 * The arguments are identical to the standard C++ main(argc, argv) params.
 * Each option is given by either its short or long name, preceeded by one or two dashes respectively.
 * If it's a flag, we simply set it to true. If not, we expect another string to follow as a parameter.
 * The first argument is, expectedly, the main program name.
 */
void CommandLineParser::parse(int argc, const char * const argv[]){
    if ( argc > 1 ){
        int i = 1;
        while ( i < argc ){
            if ( argv[i][0] == '-' ){
                if ( argv[i][1] == '-' ){
                    CommandLineParser::Option* option = findOptionByLongName(std::string(&argv[i][2]), m_d->options);
                    if ( !option )
                        THROW_EXCEPTION(CommandLineParserException, std::string("Option not found: ") + argv[i], 6);

                    if ( option->isFlag )
                        option->isSet = true;
                    else {
                        ++i;
                        if ( i == argc )
                            THROW_EXCEPTION(CommandLineParserException, std::string("No value given for option: ") + argv[i - 1], 7);
                        option->isSet = true;
                        option->value = argv[i];
                    }
                } else {
                    CommandLineParser::Option* option = findOptionByShortName(std::string(&argv[i][1]), m_d->options);
                    if ( !option )
                        THROW_EXCEPTION(CommandLineParserException, std::string("Option not found: ") + argv[i], 8);

                    if ( option->isFlag )
                        option->isSet = true;
                    else {
                        ++i;
                        if ( i == argc )
                            THROW_EXCEPTION(CommandLineParserException, std::string("No value given for option: ") + argv[i - 1], 9);
                        option->isSet = true;
                        option->value = argv[i];
                    }
                }
            } else {
                m_d->script = argv[i];
                ++i;
                break;
            }
            ++i;
        }

        while ( i < argc ){
            m_d->scriptArguments.push_back(argv[i]);
            ++i;
        }
    }
}

/**
 * \brief Returns a script argument, if it exists.
 *
 * This field is set only in case if there's a script to run in the arguments.
 * Every argument after it relates directly to the script itself.
 */
const std::string &CommandLineParser::script() const{
    return m_d->script;
}

/**
 * \brief Returns the arguments listed after a script name
 *
 * If there's a script name among the command line args, all the following arguments are
 * treated as script arguments and stored in this vector.
 */
const std::vector<std::string> &CommandLineParser::scriptArguments() const{
    return m_d->scriptArguments;
}

}// namespace
