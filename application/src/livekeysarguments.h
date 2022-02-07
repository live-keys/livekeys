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

#ifndef LVLIVEKEYSARGUMENTS_H
#define LVLIVEKEYSARGUMENTS_H

#include <QObject>
#include "live/mlnode.h"

namespace lv{

class CommandLineParser;
class LivekeysArguments{

public:
    enum Command{
        None = 0,
        Help,
        Version,
        Compile
    };

public:
    explicit LivekeysArguments(const std::string &header);
    ~LivekeysArguments();

    bool pluginInfoFlag() const;
    const QString& pluginInfoImport() const;

    bool isCommand() const;
    Command command() const;

    bool helpFlag() const;
    bool versionFlag() const;
    bool globalFlag() const;
    std::string helpString() const;

    const QStringList& layers() const;

    CommandLineParser* parser();

    const std::string &script() const;
    const std::vector<std::string>& scriptArguments() const;
    const std::vector<std::string>& arguments() const;

    void initialize(int argc, const char* const argv[]);

    const MLNode& layerConfiguration() const;
    MLNode layerConfigurationFor(const std::string& layerName) const;
    const MLNode& logConfiguration() const;
    const MLNode& compileConfiguration() const;

private:
    CommandLineParser* m_parser;
    Command m_command;

    MLNode m_compileConfiguration;
    MLNode m_logConfiguration;
    MLNode m_layerConfiguration;

    bool        m_globalScript;
    QStringList m_layers;
};

inline CommandLineParser *LivekeysArguments::parser(){
    return m_parser;
}


}// namespace

#endif // LVLIVEKEYSARGUMENTS_H
