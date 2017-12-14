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

#ifndef LIVECVARGUMENTS_H
#define LIVECVARGUMENTS_H

#include <QObject>
#include "live/mlnode.h"

namespace lv{

class CommandLineParser;
class LiveCVArguments{

public:
    explicit LiveCVArguments(const QString& header);
    ~LiveCVArguments();

    bool launchFlag() const;
    bool runFlag() const;
    bool consoleFlag() const;
    bool pluginInfoFlag() const;
    const QString& pluginInfoImport() const;

    bool helpFlag() const;
    bool versionFlag() const;
    QString helpString() const;

    const QStringList& monitoredFiles() const;

    CommandLineParser* parser();

    const QString& script() const;
    const QStringList& scriptArguments() const;

    void initialize(int argc, const char* const argv[]);

    const MLNode& getLogConfiguration();

private:
    CommandLineParser* m_parser;

    bool m_consoleFlag;
    bool m_launchFlag;
    bool m_runFlag;

    MLNode m_logConfiguration;

    bool m_pluginInfoFlag;
    QString m_pluginInfoImport;

    QStringList m_monitoredFiles;

};

inline bool LiveCVArguments::launchFlag() const{
    return m_launchFlag;
}

inline bool LiveCVArguments::runFlag() const{
    return m_runFlag;
}

inline bool LiveCVArguments::consoleFlag() const{
    return m_consoleFlag;
}

inline const QString &LiveCVArguments::pluginInfoImport() const{
    return m_pluginInfoImport;
}

inline const QStringList &LiveCVArguments::monitoredFiles() const{
    return m_monitoredFiles;
}

inline CommandLineParser *LiveCVArguments::parser(){
    return m_parser;
}


}// namespace

#endif // LIVECVARGUMENTS_H
