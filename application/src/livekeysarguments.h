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

#ifndef LVLIVEKEYSARGUMENTS_H
#define LVLIVEKEYSARGUMENTS_H

#include <QObject>
#include "live/mlnode.h"

namespace lv{

class CommandLineParser;
class LivekeysArguments{

public:
    explicit LivekeysArguments(const std::string &header);
    ~LivekeysArguments();

    bool pluginInfoFlag() const;
    const QString& pluginInfoImport() const;

    bool helpFlag() const;
    bool versionFlag() const;
    std::string helpString() const;

    const QStringList& layers() const;
    const QStringList& monitoredFiles() const;

    CommandLineParser* parser();

    const std::string &script() const;
    const std::vector<std::string>& scriptArguments() const;

    void initialize(int argc, const char* const argv[]);

    const MLNode& getLogConfiguration();

private:
    CommandLineParser* m_parser;

    MLNode m_logConfiguration;

    QStringList m_monitoredFiles;
    QStringList m_layers;
};

inline const QStringList &LivekeysArguments::monitoredFiles() const{
    return m_monitoredFiles;
}

inline CommandLineParser *LivekeysArguments::parser(){
    return m_parser;
}


}// namespace

#endif // LVLIVEKEYSARGUMENTS_H
