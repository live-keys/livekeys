#include "qlivecvarguments.h"
#include "qlivecvcommandlineparser.h"

namespace lcv{

QLiveCVArguments::QLiveCVArguments(const QString& header, int argc, const char* const argv[], QObject *parent)
    : QObject(parent)
    , m_parser(new QLiveCVCommandLineParser(header))
{
    initialize(argc, argv);
}

QLiveCVArguments::~QLiveCVArguments(){
    delete m_parser;
}

bool QLiveCVArguments::pluginInfoFlag() const{
    return m_pluginInfoFlag;
}

bool QLiveCVArguments::helpFlag() const{
    return m_parser->isSet(m_parser->helpOption());
}

QString QLiveCVArguments::at(int number) const{
    return m_parser->scriptArguments().at(number);
}

int QLiveCVArguments::length() const{
    return m_parser->scriptArguments().size();
}

QString QLiveCVArguments::option(const QString &key) const{
    QLiveCVCommandLineParser::Option* option = m_parser->findScriptOptionByName(key);
    if ( !option ){
        qCritical("Failed to find script option: %s", qPrintable(key));
        return "";
    }
    return m_parser->value(option);
}

bool QLiveCVArguments::isOptionSet(const QString &key) const{
    QLiveCVCommandLineParser::Option* option = m_parser->findScriptOptionByName(key);
    if ( !option ){
        qCritical("Failed to find script option: ", qPrintable(key));
        return "";
    }
    return m_parser->isSet(option);
}

void QLiveCVArguments::initialize(int argc, const char* const argv[]){

    QLiveCVCommandLineParser::Option* consoleOption = m_parser->addFlag(QStringList() << "-c" << "--console",
        "Output log data to console instead of using the log window.");
    QLiveCVCommandLineParser::Option* logOption     = m_parser->addFlag(QStringList() << "-l",
        "Output log data to a log file. Default file: livecv.log");
    QLiveCVCommandLineParser::Option* previewOption  = m_parser->addFlag(QStringList() << "-p" << "--preview",
        "Launches live cv in preview mode. Does not load the editor.");
    QLiveCVCommandLineParser::Option* monitorOption  = m_parser->addOption(QStringList() << "-m" << "--monitor",
        "Opens the list of paths in monitor mode.", "list");
    QLiveCVCommandLineParser::Option* pluginInfoOption = m_parser->addOption(QStringList() << "--plugininfo",
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

    m_script = m_parser->script();
}

bool QLiveCVArguments::versionFlag() const{
    return m_parser->isSet(m_parser->versionOption());
}

QString QLiveCVArguments::helpString() const{
    return m_parser->helpString();
}

}// namespace
