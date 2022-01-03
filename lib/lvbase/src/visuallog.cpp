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

#include "visuallog.h"
#include "live/mlnodetojson.h"
#include "live/utf8.h"
#include <unordered_map>
#include <QFile>
#include <QDateTime>
#include <QVariant>
#include <QSharedPointer>


/**
 * \class lv::VisualLog
 * \brief Main logging class
 *
 * The way we primarily use this class is through a predefined macro `vlog`. It's defined in the following way
 *
 * ```
 * lv::VisualLog(__VA_ARGS__).at(__FILE__, __LINE__, __FUNCTION__)
 * ```
 *
 * This means that we can pass arguments to the vlog macro that are in accordance with the constructors of VisualLog.
 * Those are:
 * ```
 * VisualLog();
 * VisualLog(MessageInfo::Level level);
 * VisualLog(const std::string& configuration);
 * VisualLog(const std::string& configuration, MessageInfo::Level level);
 * ```
 *
 * Therefore, we can pass no arguments, or we can pass a configuration string (also known as tag), or a default message level, or both.
 * The at(...) function arguments provide us with the file, line number and function name of the place we're invoking the vlog call.
 *
 * Six levels of logging are available, in order of importance: Fatal, Error, Warning, Info, Debug, Verbose.
 * There's a global configuration of the logger available, but there's also the ability to create a special configuration paired to a user-provided tag.
 * Each user-created configuration starts as a global configuration - we can then change individual parameters as desired.
 * `vlog(extension)` would create a configuration with the tag "extension". Perhaps the most important parameters of the configuration
 * are the message levels. Each configuration has an application level and a default message level. If the level of the message we're logging
 * is of less importance than the application level, it will not be displayed.
 * We can set the level of the message in the following way
 * ```
 * vlog(extension).f() << "fatal"
 * vlog(extension).e() << "error"
 * vlog(extension).w() << "warning"
 * vlog(extension).i() << "info"
 * vlog(extension).d() << "debug"
 * vlog(extension).v() << "verbose"
 * ```
 * Invoking one of the shorthand functions changes the message level of the logger. The default message level is info, while the application level
 * is debug.
 * An example on how to configure the custom configuration is given below.
 * ```
 * vlog().configure("test", {
        {"level", VisualLog::MessageInfo::Error},
        {"defaultLevel",     VisualLog::MessageInfo::Info}
    });
 * ```
 * The above would change the application level of the logger to Error, and the default message level to Info. Messages would not get displayed for this particular configuration!
 * The options are given as an Object MLNode (\sa MLNode). All the relevant options are listed below:
 * * level - application level, the least important message level that the logger's showing
 * * defaultLevel - default level of messages
 * * file - output log file
 * * logDaily - if the log should be created on a daily basis
 * * toConsole - if the log messages should be passed to the console
 * * toExtensions - if the log messages should be passed to other transports
 * * toView - if the log messages should be passed to view
 * * logObjects - set flags for places where we should log objects
 * * prefix - formatted message prefix, depends on preset values (explained below)
 *
 * These settings can also be modified through the command line arguments (see --help).
 * We can output our logged messages in several ways: `Console`, `View`, `File` and `Extensions`. It's important to mention
 * the concept of Transport, which we use an abstraction of a listener picking up on our log messages. We can treat the Console and File
 * as "transports" even though they're not technically extending our abstract class. Transport can broadcast the message in a simple textform, or
 * as an object. View Transports are used to listen to messages which can be displayed in a visual way. If the message isn't in the correct format,
 * or visual logging isn't enabled in the config, we default to a console display. Similarly, a generic transport is used for sending the object
 * to an external listener, be it a file or a network listener. If object logging isn't enabled, or the object isn't in the correct form, we once again default
 * to a console display.
 *
 * \ingroup lvbase
 */

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
   OutputDebugStringA(message.c_str());
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
                    case 'd': base += QString::asprintf("%0*d", 2, dt.date().day() ).toStdString(); break;
                    case 'e': base += QString::asprintf("%d",      dt.date().day() ).toStdString(); break;
                    case 'f': base += QString::asprintf("%*d",  2, dt.date().day() ).toStdString(); break;
                    case 'm': base += QString::asprintf("%0*d", 2, dt.date().month() ).toStdString(); break;
                    case 'n': base += QString::asprintf("%d",      dt.date().month() ).toStdString(); break;
                    case 'o': base += QString::asprintf("%*d",  2, dt.date().month() ).toStdString(); break;
                    case 'y': base += QString::asprintf("%0*d", 2, dt.date().year() % 100 ).toStdString(); break;
                    case 'Y': base += QString::asprintf("%0*d", 4, dt.date().year() ).toStdString(); break;
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
/// \private
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
    , m_logFile(nullptr)
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
    if ( m_logFile != nullptr ){
        m_logFile->close();
        delete m_logFile;
        m_logFile = nullptr;
    }
}

// VisualLog::ConfigurationContainer
// ---------------------------------------------------------------------

/// \private
class VisualLog::ConfigurationContainer{

public:
    ConfigurationContainer();

    int addConfiguration(const std::string& key, VisualLog::Configuration* configuration);

    VisualLog::Configuration* globalConfiguration();

    VisualLog::Configuration* configurationAt(const std::string& key);
    VisualLog::Configuration* configurationAt(int index);

    VisualLog::Configuration* configurationAtOrGlobal(const std::string& key);

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

VisualLog::ConfigurationContainer &VisualLog::registeredConfigurations(){
    static ConfigurationContainer registeredConfigurations = createDefaultConfigurations();
    return registeredConfigurations;
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
        return nullptr;
    return it->second;
}

VisualLog::Configuration *VisualLog::ConfigurationContainer::configurationAt(int index){
    return m_configurations.at(index);
}

VisualLog::Configuration *VisualLog::ConfigurationContainer::configurationAtOrGlobal(const std::string &key){
    auto it = m_configurationMap.find(key);
    if ( it == m_configurationMap.end() )
        return globalConfiguration();
    return it->second;
}

int VisualLog::ConfigurationContainer::configurationCount() const{
    return m_configurations.size();
}

// VisualLog
// ---------------------------------------------------------------------


VisualLog::ViewTransport* VisualLog::m_model = nullptr;

bool VisualLog::m_globalConfigured = false;

/**
 * \brief Default constructor of VisualLog
 */
VisualLog::VisualLog()
    : m_configuration(registeredConfigurations().globalConfiguration())
    , m_stream(new std::stringstream)
    , m_objectOutput(false)
{
    m_messageInfo.m_level = m_configuration->m_defaultLevel;
    init();
}

/**
 * \brief Constructor of VisualLog with level parameter
*/
VisualLog::VisualLog(VisualLog::MessageInfo::Level level)
    : m_configuration(registeredConfigurations().globalConfiguration())
    , m_messageInfo(level)
    , m_stream(new std::stringstream)
    , m_objectOutput(false)
{
    init();
}

/**
* \brief Constructor of VisualLog with configuration parameter
*/
VisualLog::VisualLog(const std::string &configurationKey)
    : m_configuration(registeredConfigurations().configurationAtOrGlobal(configurationKey))
    , m_stream(new std::stringstream)
    , m_objectOutput(false)
{
    m_messageInfo.m_level = m_configuration->m_defaultLevel;
    init();
}

/** \brief Constructor of VisualLog with both configuration and level parameters */
VisualLog::VisualLog(const std::string &configurationKey, VisualLog::MessageInfo::Level level)
    : m_configuration(registeredConfigurations().configurationAtOrGlobal(configurationKey))
    , m_messageInfo(level)
    , m_stream(new std::stringstream)
    , m_objectOutput(false)
{
    init();
}

/** \brief Destructor of VisualLog */
VisualLog::~VisualLog(){
    flushLine();
    delete m_stream;
}

/** Display enum value as string */
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

/** Return enum value from string */
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

/**
 * \brief Returns a prefix extracted from a given configuration object
 */
std::string VisualLog::MessageInfo::prefix(const VisualLog::Configuration *configuration) const{
    if ( !configuration->m_prefix.empty() ){
        return expand(configuration->m_prefix);
    }
    return "";
}

/**
 * \brief Returns the tag which is actually the name of the given configuration
 */
std::string VisualLog::MessageInfo::tag(const VisualLog::Configuration *configuration) const{
    return configuration->m_name;
}

/**
 * \brief Return the level of this message
 */
VisualLog::MessageInfo::Level VisualLog::MessageInfo::level() const{
    return m_level;
}

/** \brief Configure VisualLog given a configuration tag and options */
void VisualLog::configure(const std::string &configuration, const MLNode& options){
    m_output = 0;

    VisualLog::Configuration* cfg = registeredConfigurations().configurationAt(configuration);
    if ( !cfg ){
        cfg = new VisualLog::Configuration(*registeredConfigurations().globalConfiguration());
        cfg->m_name = configuration;
        registeredConfigurations().addConfiguration(configuration, cfg);
    }

    configure(cfg, options);
}

/** \brief Configure VisualLog given the configuration data and options */
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
                configuration->m_applicationLevel = static_cast<VisualLog::MessageInfo::Level>(it.value().asInt());
            }
        } else if ( it.key() == "defaultLevel" ){
            if ( it.value().type() == MLNode::String ){
                configuration->m_defaultLevel = VisualLog::MessageInfo::levelFromString(it.value().asString());
            } else {
                configuration->m_defaultLevel = static_cast<VisualLog::MessageInfo::Level>(it.value().asInt());
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

/** \brief Adds transport given a predefined configuration */
void VisualLog::addTransport(const std::string &configuration, VisualLog::Transport *transport){
    m_output = 0; // Disable output

    VisualLog::Configuration* cfg = registeredConfigurations().configurationAt(configuration);
    if ( !cfg ){
        cfg = new VisualLog::Configuration(*registeredConfigurations().globalConfiguration());
        cfg->m_name = configuration;
        registeredConfigurations().addConfiguration(configuration, cfg);
    }

    addTransport(cfg, transport);
}

/** \brief Adds transport given configuration data */
void VisualLog::addTransport(VisualLog::Configuration *configuration, VisualLog::Transport *transport){
    m_output = 0; // Disable output

    configuration->m_transports.append(QSharedPointer<VisualLog::Transport>(transport));
}

/** \brief Removes transport given a predefined configuration */
void VisualLog::removeTransports(const std::string &configuration){
    m_output = 0; // Disable output

    VisualLog::Configuration* cfg = registeredConfigurations().configurationAt(configuration);
    if ( cfg )
        removeTransports(cfg);
}

/** \brief Removes transport given configuration data */
void VisualLog::removeTransports(VisualLog::Configuration *configuration){
    m_output = 0; // Disable output

    configuration->m_transports.clear();
}

/** \brief Returns total number of configurations */
int VisualLog::totalConfigurations(){
    m_output = 0;

    return registeredConfigurations().configurationCount();
}

/** \brief Flushes the entire buffer to preset outputs */
void VisualLog::flushLine(){
    if ( canLog() ){
        std::string pref = prefix();
        std::string buffer = m_stream->str();
        if ( m_output & VisualLog::Console )
            vLoggerConsole(pref + buffer + "\n");
        if ( m_output & VisualLog::File )
            flushFile(pref + buffer + "\n");
        if ( m_output & VisualLog::View && m_model )
            m_model->onMessage(m_configuration, m_messageInfo, buffer);
        if ( m_output & VisualLog::Extensions )
            flushHandler(buffer);

        m_stream->clear();
    }
}

/** \brief Closes the internal log file */
void VisualLog::closeFile(){
    m_output = 0; // Disable output
    m_configuration->closeFile();
}

/** \brief Display viewData in the View given by the viewPath */
void VisualLog::asView(const std::string &viewPath, const QVariant &viewData){
    if ( canLog() && m_objectOutput && (m_output & VisualLog::View) ){
        m_model->onView(m_configuration, m_messageInfo, viewPath, viewData);
        m_output = removeOutputFlag(m_output, VisualLog::View);
    }
}

/** \brief Display view data returned by the given function in the View given by the viewPath */
void VisualLog::asView(const std::string &viewPath, std::function<QVariant ()> cloneFunction){
    if ( canLog() && m_objectOutput && (m_output & VisualLog::View) ){
        m_model->onView(m_configuration, m_messageInfo, viewPath, cloneFunction());
        m_output = removeOutputFlag(m_output, VisualLog::View);
    }
}

/** \brief Display MLNode as object of given type */
void VisualLog::asObject(const std::string &type, const MLNode &mlvalue){
    std::string str;
    ml::toJson(mlvalue, str);
    std::string pref = prefix();
    std::string writeData =
        pref + "\\@" + type + "\n" +
        std::string(pref.length(), ' ') + str + "\n";

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

/** View transport model */
VisualLog::ViewTransport *VisualLog::model(){
    return m_model;
}

/** Sets view transport model */
void VisualLog::setViewTransport(VisualLog::ViewTransport *model){
    m_model = model;
}

/** Initialize the output from the configuration */
void VisualLog::init(){
    m_output = m_configuration->m_output;
}

void VisualLog::flushFile(const std::string& data){
    if ( m_configuration->m_logDaily ){
        QDateTime cdt = m_messageInfo.stamp();
        if ( cdt.date() != m_configuration->m_lastLog || m_configuration->m_logFile == nullptr ){
            m_configuration->m_logFile = new QFile(QString::fromStdString(visualLogDateFormat(m_configuration->m_filePath, cdt)));
            if ( !m_configuration->m_logFile->open(QIODevice::Append) ){
                m_configuration->m_output = removeOutputFlag(m_configuration->m_output, VisualLog::File);
                std::string fileName = m_configuration->m_logFile->fileName().toStdString();
                qCritical("Failed to open file: \'%s\'. Closing file output stream.", fileName.c_str());
                return;
            }
        }
    } else if ( m_configuration->m_logFile == nullptr ){
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

/** Flush data to console */
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

                    base << QString::asprintf(
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
                case 'd': base << QString::asprintf("%0*d", 2, dt.date().day() ).toStdString(); break;
                case 'e': base << QString::asprintf("%d",      dt.date().day() ).toStdString(); break;
                case 'f': base << QString::asprintf("%*d",  2, dt.date().day() ).toStdString(); break;
                case 'm': base << QString::asprintf("%0*d", 2, dt.date().month() ).toStdString(); break;
                case 'n': base << QString::asprintf("%d",      dt.date().month() ).toStdString(); break;
                case 'o': base << QString::asprintf("%*d",  2, dt.date().month() ).toStdString(); break;
                case 'y': base << QString::asprintf("%0*d", 2, dt.date().year() % 100 ).toStdString(); break;
                case 'Y': base << QString::asprintf("%0*d", 4, dt.date().year() ).toStdString(); break;
                case 'H': base << QString::asprintf("%0*d", 2, dt.time().hour() ).toStdString(); break;
                case 'I': {
                    int hour = dt.time().hour();
                    base << QString::asprintf("%0*d", 2, (hour < 1 ? 12 : (hour > 12 ? hour - 12  : hour))).toStdString();
                    break;
                }
                case 'a': base << QString::asprintf(dt.time().hour() < 12  ? "am" : "pm" ).toStdString(); break;
                case 'A': base << QString::asprintf(dt.time().hour() < 12  ? "AM" : "PM" ).toStdString(); break;
                case 'M': base << QString::asprintf("%0*d", 2, dt.time().minute()).toStdString(); break;
                case 'S': base << QString::asprintf("%0*d", 2, dt.time().second() ).toStdString(); break;
                case 's': base << QString::asprintf("%0*d.%0*d", 2, dt.time().second(), 3, dt.time().msec() ).toStdString(); break;
                case 'i': base << QString::asprintf("%0*d", 3, dt.time().msec() ).toStdString(); break;
                case 'c': base << QString::asprintf("%d",      dt.time().msec() / 100 ).toStdString(); break;
                default: base << *it;
                }
            } else {
                base << '%';
                break;
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

/** \brief Shows if logging is enabled */
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

/**
  \class lv::VisualLog::MessageInfo
  \brief Simple wrapper class for log messages, including the level, timestamp and location info

  Location info includes a remote, function name, line number and file name. All of these are optional.
  \ingroup lvbase
 */

/**
 * \brief MessageInfo desctructor
 *
 * Deletes location and stamp pointers.
 */
VisualLog::MessageInfo::~MessageInfo(){
    delete m_location;
    delete m_stamp;
}

VisualLog::MessageInfo::MessageInfo()
    : m_level(MessageInfo::Info)
    , m_location(nullptr)
    , m_stamp(nullptr)
{
}

VisualLog::MessageInfo::MessageInfo(VisualLog::MessageInfo::Level level)
    : m_level(level)
    , m_location(nullptr)
    , m_stamp(nullptr)
{
}

/** \brief Overrides the previous timestamp with the given one */
VisualLog &lv::VisualLog::overrideStamp(const QDateTime &stamp){
    if ( !m_messageInfo.m_stamp )
        m_messageInfo.m_stamp = new QDateTime(stamp);
    else
        *m_messageInfo.m_stamp = stamp;
    return *this;
}

/**
 * \brief Returns the message info timestamp
 *
 * If there's none, it takes the current time and sets it as the stamp.
 */
const QDateTime &lv::VisualLog::MessageInfo::stamp() const{
    if ( !m_stamp )
        m_stamp = new QDateTime(QDateTime::currentDateTime());
    return *m_stamp;
}

}// namespace

