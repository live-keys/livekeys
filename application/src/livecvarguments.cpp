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

#include "livecvarguments.h"
#include "live/commandlineparser.h"

namespace lcv{

LiveCVArguments::LiveCVArguments(const QString& header, int argc, const char* const argv[])
    : m_parser(new CommandLineParser(header))
{
    initialize(argc, argv);
}

LiveCVArguments::~LiveCVArguments(){
    delete m_parser;
}

bool LiveCVArguments::pluginInfoFlag() const{
    return m_pluginInfoFlag;
}

bool LiveCVArguments::helpFlag() const{
    return m_parser->isSet(m_parser->helpOption());
}

void LiveCVArguments::initialize(int argc, const char* const argv[]){

    CommandLineParser::Option* consoleOption = m_parser->addFlag(QStringList() << "-c" << "--console",
        "Output log data to console instead of using the log window.");
    CommandLineParser::Option* logOption     = m_parser->addFlag(QStringList() << "-l",
        "Output log data to a log file. Default file: livecv.log");
    CommandLineParser::Option* previewOption  = m_parser->addFlag(QStringList() << "-p" << "--preview",
        "Launches live cv in preview mode. Does not load the editor.");
    CommandLineParser::Option* monitorOption  = m_parser->addOption(QStringList() << "-m" << "--monitor",
        "Opens the list of paths in monitor mode.", "list");
    CommandLineParser::Option* pluginInfoOption = m_parser->addOption(QStringList() << "--plugininfo",
        "Outputs the plugin info to a specified import (e.g. --plugininfo \"lcvcore 1.0\".", "string");

    m_parser->parse(argc, argv);

    m_consoleFlag    = m_parser->isSet(consoleOption);
    m_fileLogFlag    = m_parser->isSet(logOption);
    m_previewFlag    = m_parser->isSet(previewOption);
    m_pluginInfoFlag = m_parser->isSet(pluginInfoOption);
    m_pluginInfoImport = m_parser->value(pluginInfoOption);

    QString monitoredList = m_parser->value(monitorOption);
    if ( !monitoredList.isEmpty() ){
        m_monitoredFiles = monitoredList.split(";");
    }

}

bool LiveCVArguments::versionFlag() const{
    return m_parser->isSet(m_parser->versionOption());
}

QString LiveCVArguments::helpString() const{
    return m_parser->helpString();
}

const QStringList &LiveCVArguments::scriptArguments() const{
    return m_parser->scriptArguments();
}

const QString &LiveCVArguments::script() const{
    return m_parser->script();
}

}// namespace
