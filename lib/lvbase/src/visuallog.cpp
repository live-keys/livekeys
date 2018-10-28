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

#include "visuallog.h"
#include "live/mlnodetojson.h"
#include <unordered_map>
#include <QFile>
#include <QDateTime>
#include <QVariant>
#include <QSharedPointer>


#if defined(Q_OS_UNIX) || (defined(Q_OS_WIN) && defined(QS_LOG_WIN_PRINTF_CONSOLE))
#include <cstdio>
void vLoggerConsole(const std::string& message)
{
   fprintf(stderr, "%s", message.c_str());
   fflush(stderr);
}
#elif defined(Q_OS_WIN)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
void vLoggerConsole(const std::string& message)
{
   OutputDebugStringW(reinterpret_cast<const WCHAR*>(message.utf16()));
}
#endif

namespace lv{

namespace{

    std::string visualLogDateFormat(const std::string& format, const QDateTime& dt){
        std::string base = "";
        char c = 0;

        std::string::const_iterator it = format.begin();
        while ( it != format.end() ){
            if ( c == '%' ){
                ++it;
                if ( it != format.end() ){
                    char c = *it;
                    switch(c){ //TODO: Optimize conversions
                    case 'w': base += QDate::shortDayName(dt.date().dayOfWeek()).toStdString(); break;
                    case 'W': base += QDate::longDayName(dt.date().dayOfWeek()).toStdString(); break;
                    case 'b': base += QDate::shortMonthName(dt.date().month()).toStdString(); break;
                    case 'B': base += QDate::longMonthName(dt.date().month()).toStdString(); break;
                    case 'd': base += QString().sprintf("%0*d", 2, dt.date().day() ).toStdString(); break;
                    case 'e': base += QString().sprintf("%d",      dt.date().day() ).toStdString(); break;
                    case 'f': base += QString().sprintf("%*d",  2, dt.date().day() ).toStdString(); break;
                    case 'm': base += QString().sprintf("%0*d", 2, dt.date().month() ).toStdString(); break;
                    case 'n': base += QString().sprintf("%d",      dt.date().month() ).toStdString(); break;
                    case 'o': base += QString().sprintf("%*d",  2, dt.date().month() ).toStdString(); break;
                    case 'y': base += QString().sprintf("%0*d", 2, dt.date().year() % 100 ).toStdString(); break;
                    case 'Y': base += QString().sprintf("%0*d", 4, dt.date().year() ).toStdString(); break;
                    default: base += *it;
                    }
                }
            } else {
                base += *it;
            }
            ++it;
        }

        return base;
    }

    std::string extractFileNameSegment(const std::string& file){
        std::string::size_type pos = file.rfind('/');

        #ifdef Q_OS_WIN
            if ( pos == std::string::npos ){
                pos = file.rfind('\\');
            }
        #endif

        if ( pos != std::string::npos )
            return file.substr(pos + 1);

        return file;
    }

    std::string asciiToLower(const std::string& str){
        std::string base;
        base.reserve(str.size());
        for ( const char& c : str ){
            base.push_back(tolower(c));
        }
        return base;
    }
} // namespace



// VisualLog::Configuration
// ---------------------------------------------------------------------

class VisualLog::Configuration{

public:
    Configuration(
        const std::string& name,
        VisualLog::MessageInfo::Level applicationLevel,
        VisualLog::MessageInfo::Level defaultLevel,
        const std::string& filePath,
        bool dailyFile);
    Configuration(
        const std::string& name,
        const Configuration& other
    );

    void closeFile();

public:
    std::string        m_name;
    VisualLog::MessageInfo::Level m_applicationLevel;
    VisualLog::MessageInfo::Level m_defaultLevel;
    std::string        m_filePath;
    int            m_output;
    int            m_logObjects;
    bool           m_logDaily;
    QFile*         m_logFile;
    QDate          m_lastLog;
    std::string    m_prefix;

    QList<QSharedPointer<VisualLog::Transport> > m_transports;
};


VisualLog::Configuration::Configuration(
        const std::string &name,
        VisualLog::MessageInfo::Level applicationLevel,
        VisualLog::MessageInfo::Level defaultLevel,
        const std::string &filePath,
        bool dailyFile)
    : m_name(name)
    , m_applicationLevel(applicationLevel)
    , m_defaultLevel(defaultLevel)
    , m_filePath(filePath)
    , m_output(VisualLog::Console | VisualLog::View | VisualLog::Extensions)
    , m_logObjects(VisualLog::File | VisualLog::Extensions)
    , m_logDaily(dailyFile)
    , m_logFile(0)
    , m_transports()
{}

VisualLog::Configuration::Configuration(const std::string &name, const VisualLog::Configuration &other)
    : m_name(name)
    , m_applicationLevel(other.m_applicationLevel)
    , m_defaultLevel(other.m_defaultLevel)
    , m_filePath(other.m_filePath)
    , m_output(other.m_output)
    , m_logObjects(other.m_logObjects)
    , m_logDaily(other.m_logDaily)
    , m_logFile(other.m_logFile)
    , m_prefix(other.m_prefix)
    , m_transports(other.m_transports)
{
}

void VisualLog::Configuration::closeFile(){
    if ( m_logFile != 0 ){
        m_logFile->close();
        delete m_logFile;
        m_logFile = 0;
    }
}

// VisualLog::ConfigurationContainer
// ---------------------------------------------------------------------


class VisualLog::ConfigurationContainer{

public:
    ConfigurationContainer();

    int addConfiguration(const std::string& key, VisualLog::Configuration* configuration);

    VisualLog::Configuration* globalConfiguration();

    VisualLog::Configuration* configurationAt(const std::string& key);
    VisualLog::Configuration* configurationAt(int index);

    int configurationCount() const;

private:
    QList<VisualLog::Configuration*>          m_configurations;
    std::unordered_map<std::string, VisualLog::Configuration*> m_configurationMap;
};

VisualLog::ConfigurationContainer VisualLog::createDefaultConfigurations(){
    VisualLog::ConfigurationContainer container;

    VisualLog::Configuration* configuration = new VisualLog::Configuration(
        "global",
        VisualLog::MessageInfo::Level::Debug,
        VisualLog::MessageInfo::Level::Info,
        "",
        false
    );

    container.addConfiguration("global", configuration);

    return container;
}

VisualLog::ConfigurationContainer::ConfigurationContainer(){
}

int VisualLog::ConfigurationContainer::addConfiguration(const std::string &key, VisualLog::Configuration *configuration){
    Q_ASSERT(m_configurationMap.find(key) == m_configurationMap.end());

    m_configurations.append(configuration);
    m_configurationMap[key] = configuration;

    return m_configurations.size();
}

VisualLog::Configuration *VisualLog::ConfigurationContainer::globalConfiguration(){
    return m_configurations.front();
}

VisualLog::Configuration *VisualLog::ConfigurationContainer::configurationAt(const std::string &key){
    auto it = m_configurationMap.find(key);
    if ( it == m_configurationMap.end() )
        return 0;
    return it->second;;
}

VisualLog::Configuration *VisualLog::ConfigurationContainer::configurationAt(int index){
    return m_configurations.at(index);
}

int VisualLog::ConfigurationContainer::configurationCount() const{
    return m_configurations.size();
}


// VisualLog::Location
// ---------------------------------------------------------------------

VisualLog::SourceLocation::SourceLocation(const std::string &pfile, int pline, const std::string &pFunctionName)
    : file(pfile)
    , line(pline)
    , functionName(pFunctionName)
{
}

VisualLog::SourceLocation::SourceLocation(
        const std::string &premote,
        const std::string &pfile,
        int pline,
        const std::string &pfunctionName)
    : remote(premote)
    , file(pfile)
    , line(pline)
    , functionName(pfunctionName)
{
}

// VisualLog
// ---------------------------------------------------------------------


VisualLog::ConfigurationContainer VisualLog::m_registeredConfigurations = VisualLog::createDefaultConfigurations();
VisualLog::ViewTransport* VisualLog::m_model = 0;

bool VisualLog::m_globalConfigured = false;

VisualLog::VisualLog()
    : m_configuration(m_registeredConfigurations.globalConfiguration())
    , m_objectOutput(false)
{
    m_messageInfo.m_level = m_configuration->m_defaultLevel;
    init();
}

VisualLog::VisualLog(VisualLog::MessageInfo::Level level)
    : m_configuration(m_registeredConfigurations.globalConfiguration())
    , m_messageInfo(level)
    , m_objectOutput(false)
{
    init();
}

VisualLog::VisualLog(const std::string &configurationKey)
    : m_configuration(m_registeredConfigurations.configurationAt(configurationKey))
    , m_objectOutput(false)
{
    m_messageInfo.m_level = m_configuration->m_defaultLevel;
    init();
}

VisualLog::VisualLog(const std::string &configurationKey, VisualLog::MessageInfo::Level level)
    : m_configuration(m_registeredConfigurations.configurationAt(configurationKey))
    , m_messageInfo(level)
    , m_objectOutput(false)
{
    init();
}

VisualLog::~VisualLog(){
    flushLine();
}

std::string VisualLog::MessageInfo::levelToString(VisualLog::MessageInfo::Level level){
    switch(level){
    case VisualLog::MessageInfo::Level::Fatal: return "Fatal";
    case VisualLog::MessageInfo::Level::Error: return "Error";
    case VisualLog::MessageInfo::Level::Warning: return "Warning";
    case VisualLog::MessageInfo::Level::Info: return "Info";
    case VisualLog::MessageInfo::Level::Debug: return "Debug";
    case VisualLog::MessageInfo::Level::Verbose: return "Verbose";
    }
    return std::string();
}

VisualLog::MessageInfo::Level VisualLog::MessageInfo::levelFromString(const std::string &str){
    std::string strlow = asciiToLower(str);
    if ( strlow == "fatal" ){
        return VisualLog::MessageInfo::Level::Fatal;
    } else if ( strlow == "error" ){
        return VisualLog::MessageInfo::Level::Error;
    } else if ( strlow == "warning" ){
        return VisualLog::MessageInfo::Level::Warning;
    } else if ( strlow == "info" ){
        return VisualLog::MessageInfo::Level::Info;
    } else if ( strlow == "debug" ){
        return VisualLog::MessageInfo::Level::Debug;
    } else if ( strlow == "verbose" ){
        return VisualLog::MessageInfo::Level::Verbose;
    }
    return VisualLog::MessageInfo::Level::Info;
}

std::string VisualLog::MessageInfo::prefix(const VisualLog::Configuration *configuration) const{
    if ( !configuration->m_prefix.empty() ){
        return expand(configuration->m_prefix);
    }
    return "";
}

std::string VisualLog::MessageInfo::tag(const VisualLog::Configuration *configuration) const{
    return configuration->m_name;
}

void VisualLog::configure(const std::string &configuration, const MLNode& options){
    m_output = 0;

    VisualLog::Configuration* cfg = m_registeredConfigurations.configurationAt(configuration);
    if ( !cfg ){
        cfg = new VisualLog::Configuration(*m_registeredConfigurations.globalConfiguration());
        cfg->m_name = configuration;
        m_registeredConfigurations.addConfiguration(configuration, cfg);
    }

    configure(cfg, options);
}

void VisualLog::configure(VisualLog::Configuration *configuration, const MLNode &options){
    m_output = 0; // Disable output

    if ( !configuration ){
        THROW_EXCEPTION(Exception, "Null configuration given", 0);
    }

    if ( configuration->m_name == "global" ){
        if ( m_globalConfigured ){
            THROW_EXCEPTION(Exception, "Cannot reconfigure global configuration.", 0);
        }
        m_globalConfigured = true;
    }

    for ( auto it = options.begin(); it != options.end(); ++it ){
        if ( it.key() == "level" ){
            if ( it.value().type() == MLNode::String ){
                configuration->m_applicationLevel = VisualLog::MessageInfo::levelFromString(it.value().asString());
            } else {
                configuration->m_applicationLevel = (VisualLog::MessageInfo::Level)it.value().asInt();
            }
        } else if ( it.key() == "defaultLevel" ){
            if ( it.value().type() == MLNode::String ){
                configuration->m_defaultLevel = VisualLog::MessageInfo::levelFromString(it.value().asString());
            } else {
                configuration->m_defaultLevel = (VisualLog::MessageInfo::Level)it.value().asInt();
            }
        } else if ( it.key() == "file" ){
            std::string v = it.value().asString();
            if ( configuration->m_filePath != v ){
                configuration->closeFile();
                configuration->m_filePath = v;
                if ( !configuration->m_filePath.empty() ){
                    configuration->m_output = configuration->m_output | VisualLog::File;
                } else {
                    configuration->m_output = removeOutputFlag(configuration->m_output, VisualLog::File);
                }
            }
        } else if ( it.key() == "logDaily" ){
            configuration->m_logDaily = it.value().asBool();
        } else if ( it.key() == "toConsole" ){
            bool toConsole = it.value().asBool();
            if ( toConsole ){
                configuration->m_output = configuration->m_output | VisualLog::Console;
            } else {
                configuration->m_output = removeOutputFlag(configuration->m_output, VisualLog::Console);
            }
        } else if ( it.key() == "toExtensions"){
            bool toExtensions = it.value().asBool();
            if ( toExtensions ){
                configuration->m_output = configuration->m_output | VisualLog::Extensions;
            } else {
                configuration->m_output = removeOutputFlag(configuration->m_output, VisualLog::Extensions);
            }
        } else if ( it.key() == "toView" ){
            bool toView = it.value().asBool();
            if ( toView ){
                configuration->m_output = configuration->m_output | VisualLog::View;
            } else {
                configuration->m_output = removeOutputFlag(configuration->m_output, VisualLog::View);
            }
        } else if ( it.key() == "logObjects" ){
            configuration->m_logObjects = it.value().asInt();
        } else if ( it.key() == "prefix" ){
            configuration->m_prefix = it.value().asString();
        } else {
            qWarning("Unknown configuration key: %s", it.key().c_str());
        }
    }

    //TODO: Requires parameter validation checking (e.g. log file / path exists)
}

void VisualLog::addTransport(const std::string &configuration, VisualLog::Transport *transport){
    m_output = 0; // Disable output

    VisualLog::Configuration* cfg = m_registeredConfigurations.configurationAt(configuration);
    if ( !cfg ){
        cfg = new VisualLog::Configuration(*m_registeredConfigurations.globalConfiguration());
        cfg->m_name = configuration;
        m_registeredConfigurations.addConfiguration(configuration, cfg);
    }

    addTransport(cfg, transport);
}

void VisualLog::addTransport(VisualLog::Configuration *configuration, VisualLog::Transport *transport){
    m_output = 0; // Disable output

    configuration->m_transports.append(QSharedPointer<VisualLog::Transport>(transport));
}

void VisualLog::removeTransports(const std::string &configuration){
    m_output = 0; // Disable output

    VisualLog::Configuration* cfg = m_registeredConfigurations.configurationAt(configuration);
    if ( cfg )
        removeTransports(cfg);
}

void VisualLog::removeTransports(VisualLog::Configuration *configuration){
    m_output = 0; // Disable output

    configuration->m_transports.clear();
}

int VisualLog::totalConfigurations(){
    m_output = 0;

    return m_registeredConfigurations.configurationCount();
}

void VisualLog::flushLine(){
    if ( canLog() ){
        std::string pref = prefix();
        std::string buffer = m_stream.str();
        if ( m_output & VisualLog::Console )
            vLoggerConsole(pref + buffer + "\n");
        if ( m_output & VisualLog::File )
            flushFile(pref + buffer + "\n");
        if ( m_output & VisualLog::View && m_model )
            m_model->onMessage(m_configuration, m_messageInfo, buffer);
        if ( m_output & VisualLog::Extensions )
            flushHandler(buffer);

        m_stream.clear();
    }
}

void VisualLog::closeFile(){
    m_output = 0; // Disable output
    m_configuration->closeFile();
}

void VisualLog::asView(const std::string &viewPath, const QVariant &viewData){
    if ( canLog() && m_objectOutput && (m_output & VisualLog::View) ){
        m_model->onView(m_configuration, m_messageInfo, viewPath, viewData);
        m_output = removeOutputFlag(m_output, VisualLog::View);
    }
}

void VisualLog::asView(const std::string &viewPath, std::function<QVariant ()> cloneFunction){
    if ( canLog() && m_objectOutput && (m_output & VisualLog::View) ){
        m_model->onView(m_configuration, m_messageInfo, viewPath, cloneFunction());
        m_output = removeOutputFlag(m_output, VisualLog::View);
    }
}

void VisualLog::asObject(const std::string &type, const MLNode &mlvalue){
    //TODO: Use string here
    QByteArray str;
    ml::toJson(mlvalue, str);
    std::string pref = prefix();
    std::string writeData =
        pref + "\\@" + type + "\n" +
        std::string(pref.length(), ' ') + str.data() + "\n";

    if ( m_output & VisualLog::Console && m_configuration->m_logObjects & VisualLog::Console ){
        flushConsole(writeData);
        m_output &= ~VisualLog::Console; // remove console flag from text based logging
    }
    if ( m_output & VisualLog::File && m_configuration->m_logObjects & VisualLog::File ){
        flushFile(writeData);
        m_output &= ~VisualLog::File; // remove file flag from text based logging
    }
    if ( m_output & VisualLog::Extensions && m_configuration->m_logObjects & VisualLog::Extensions){
        for ( auto it = m_configuration->m_transports.begin(); it != m_configuration->m_transports.end(); ++it ){
            (*it)->onObject(m_configuration, m_messageInfo, type, mlvalue);
        }
    }
}

VisualLog::ViewTransport *VisualLog::model(){
    return m_model;
}

void VisualLog::setViewTransport(VisualLog::ViewTransport *model){
    m_model = model;
}

void VisualLog::init(){
    m_output = m_configuration->m_output;
}

void VisualLog::flushFile(const std::string& data){
    if ( m_configuration->m_logDaily ){
        QDateTime cdt = m_messageInfo.stamp();
        if ( cdt.date() != m_configuration->m_lastLog || m_configuration->m_logFile == 0 ){
            m_configuration->m_logFile = new QFile(QString::fromStdString(visualLogDateFormat(m_configuration->m_filePath, cdt)));
            if ( !m_configuration->m_logFile->open(QIODevice::Append) ){
                m_configuration->m_output = removeOutputFlag(m_configuration->m_output, VisualLog::File);
                std::string fileName = m_configuration->m_logFile->fileName().toStdString();
                qCritical("Failed to open file: \'%s\'. Closing file output stream.", fileName.c_str());
                return;
            }
        }
    } else if ( m_configuration->m_logFile == 0 ){
        m_configuration->m_logFile = new QFile(QString::fromStdString(m_configuration->m_filePath));
        if ( !m_configuration->m_logFile->open(QIODevice::Append) ){
            m_configuration->m_output = removeOutputFlag(m_configuration->m_output, VisualLog::File);
            std::string fileName = m_configuration->m_logFile->fileName().toStdString();
            qCritical("Failed to open file: \'%s\'. Closing file output stream.", fileName.c_str());
            return;
        }
    }

    m_configuration->m_logFile->write(data.c_str());
    m_configuration->m_logFile->flush();
}

void VisualLog::flushHandler(const std::string &data){
    if ( !m_configuration->m_transports.isEmpty() ){
        for ( auto it = m_configuration->m_transports.begin(); it != m_configuration->m_transports.end(); ++it ){
            (*it)->onMessage(m_configuration, m_messageInfo, data);
        }
    }
}

void VisualLog::flushConsole(const std::string &data){
    vLoggerConsole(data);
}

std::string VisualLog::MessageInfo::expand(const std::string &pattern) const{

    std::stringstream base;
    QDateTime dt = stamp();

    std::string::const_iterator it = pattern.begin();
    while ( it != pattern.end() ){
        if ( *it == '%' ){
            ++it;
            if ( it != pattern.end() ){
                switch(*it){
                case 'p': {
                    if ( m_location && !m_location->remote.empty() )
                        base << m_location->remote + "> ";

                    std::string levelToLower = asciiToLower(levelToString(m_level));

                    base << QString().sprintf(
                        "%0*d-%0*d-%0*d %0*d:%0*d:%0*d.%0*d %s %s@%d: ",
                        4, dt.date().year(),
                        2, dt.date().month(),
                        2, dt.date().day(),
                        2, dt.time().hour(),
                        2, dt.time().minute(),
                        2, dt.time().second(),
                        3, dt.time().msec(),
                        levelToLower.c_str(),
                        sourceFunctionName().c_str(),
                        sourceLineNumber()
                    ).toStdString();
                    break;
                }
                case 'r': base << sourceRemoteLocation(); break;
                case 'F': base << sourceFileName(); break;
                case 'N': base << extractFileNameSegment(sourceFileName()); break;
                case 'U': base << sourceFunctionName(); break;
                case 'L': base << sourceLineNumber(); break;
                case 'V': base << levelToString(m_level); break;
                case 'v': base << asciiToLower(levelToString(m_level)); break;
                case 'w': base << QDate::shortDayName(dt.date().dayOfWeek()).toStdString(); break;
                case 'W': base << QDate::longDayName(dt.date().dayOfWeek()).toStdString(); break;
                case 'b': base << QDate::shortMonthName(dt.date().month()).toStdString(); break;
                case 'B': base << QDate::longMonthName(dt.date().month()).toStdString(); break;
                case 'd': base << QString().sprintf("%0*d", 2, dt.date().day() ).toStdString(); break;
                case 'e': base << QString().sprintf("%d",      dt.date().day() ).toStdString(); break;
                case 'f': base << QString().sprintf("%*d",  2, dt.date().day() ).toStdString(); break;
                case 'm': base << QString().sprintf("%0*d", 2, dt.date().month() ).toStdString(); break;
                case 'n': base << QString().sprintf("%d",      dt.date().month() ).toStdString(); break;
                case 'o': base << QString().sprintf("%*d",  2, dt.date().month() ).toStdString(); break;
                case 'y': base << QString().sprintf("%0*d", 2, dt.date().year() % 100 ).toStdString(); break;
                case 'Y': base << QString().sprintf("%0*d", 4, dt.date().year() ).toStdString(); break;
                case 'H': base << QString().sprintf("%0*d", 2, dt.time().hour() ).toStdString(); break;
                case 'I': {
                    int hour = dt.time().hour();
                    base << QString().sprintf("%0*d", 2, (hour < 1 ? 12 : (hour > 12 ? hour - 12  : hour))).toStdString();
                    break;
                }
                case 'a': base << QString().sprintf(dt.time().hour() < 12  ? "am" : "pm" ).toStdString(); break;
                case 'A': base << QString().sprintf(dt.time().hour() < 12  ? "AM" : "PM" ).toStdString(); break;
                case 'M': base << QString().sprintf("%0*d", 2, dt.time().minute()).toStdString(); break;
                case 'S': base << QString().sprintf("%0*d", 2, dt.time().second() ).toStdString(); break;
                case 's': base << QString().sprintf("%0*d.%0*d", 2, dt.time().second(), 3, dt.time().msec() ).toStdString(); break;
                case 'i': base << QString().sprintf("%0*d", 3, dt.time().msec() ).toStdString(); break;
                case 'c': base << QString().sprintf("%d",      dt.time().msec() / 100 ).toStdString(); break;
                default: base << *it;
                }
            }
        } else {
            base << *it;
        }
        ++it;
    }

    return base.str();
}

int VisualLog::removeOutputFlag(int flags, VisualLog::Output output){
    if ( flags & output ){
        flags &= ~output;
    }
    return flags;
}

bool VisualLog::canLog(){
    return m_messageInfo.m_level <= m_configuration->m_applicationLevel;
}

std::string VisualLog::prefix(){
    if ( !m_configuration->m_prefix.empty() ){
        return m_messageInfo.expand(m_configuration->m_prefix);
    }
    return "";
}

bool VisualLog::canLogObjects(VisualLog::Configuration *configuration){
    return configuration->m_logObjects != 0;
}

VisualLog::MessageInfo::~MessageInfo(){
    delete m_location;
    delete m_stamp;
}

VisualLog::MessageInfo::MessageInfo()
    : m_level(MessageInfo::Info)
    , m_location(0)
    , m_stamp(0)
{
}

VisualLog::MessageInfo::MessageInfo(VisualLog::MessageInfo::Level level)
    : m_level(level)
    , m_location(0)
    , m_stamp(0)
{
}

VisualLog &lv::VisualLog::overrideStamp(const QDateTime &stamp){
    if ( !m_messageInfo.m_stamp )
        m_messageInfo.m_stamp = new QDateTime(stamp);
    else
        *m_messageInfo.m_stamp = stamp;
    return *this;
}

const QDateTime &lv::VisualLog::MessageInfo::stamp() const{
    if ( !m_stamp )
        m_stamp = new QDateTime(QDateTime::currentDateTime());
    return *m_stamp;
}

}// namespace
