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

#include "livekeysarguments.h"
#include "live/commandlineparser.h"
#include "live/mlnodetojson.h"
#include "live/visuallogqt.h"
#include <QFile>

namespace lv{

LivekeysArguments::LivekeysArguments(const std::string& header)
    : m_parser(new CommandLineParser(header))
    , m_command(LivekeysArguments::None)
    , m_globalScript(false)
{
}

LivekeysArguments::~LivekeysArguments(){
    delete m_parser;
}

bool LivekeysArguments::isCommand() const{
    return m_command != LivekeysArguments::None;
}

LivekeysArguments::Command LivekeysArguments::command() const{
    return m_command;
}

bool LivekeysArguments::helpFlag() const{
    return m_parser->isSet(m_parser->helpOption());
}

bool LivekeysArguments::versionFlag() const{
    return m_parser->isSet(m_parser->versionOption());
}

void LivekeysArguments::initialize(int argc, const char* const argv[]){
    CommandLineParser::Option* logToConsoleOption = m_parser->addFlag({"-c", "--log-toconsole"},
        "Output log data to the console.");
    CommandLineParser::Option* logLevelOption = m_parser->addOption({"--log-level"},
        "Log level for the application (Fatal|Error|Warning|Info|Debug|Verbose).", "level");
    CommandLineParser::Option* logToNetworkOption = m_parser->addOption({"--log-tonetwork"},
        "Stream log data to the network through TCP/IP.", "url");
    CommandLineParser::Option* logToNoViewOption = m_parser->addFlag({"--log-noview"},
        "Disable logging to the Livekeyss logger.");
    CommandLineParser::Option* logFileOption = m_parser->addOption({"-o", "--log-file"},
        "Output log data to a log file.", "path");
    CommandLineParser::Option* logDailyOption = m_parser->addFlag({"--log-daily"},
        "Create daily log files. The \'--log-file\' option is used as the pattern to generate the actual files."
        "The pattern is specific to Livekeyss log date format (i.e. path/to/logfile_%Y_%m_%d.txt). "
        "See the documentation on log prefixes for more details).");
    CommandLineParser::Option* logPrefixOption = m_parser->addOption({"--log-prefix"},
        "Default prefix for messages. See the documentation on logging for more info. To use the standard prefix, "
        "use (\'--log-prefix %p\').", "string");
    CommandLineParser::Option* logConfigOption = m_parser->addOption({"--log-config"},
        "Custom configuration for each log parameter. This will cancel out all other log flags. See the "
        "documentation on logging for more info.", "string");

    CommandLineParser::Option* logConfigFileOption = m_parser->addOption({"--log-config-file"},
        "Json file configuration for each log parameter. This will cancel out all other log flags. See the "
        "documentation on logging for more inf.", "path");

    CommandLineParser::Option* layers = m_parser->addOption({"-l", "--layers"},
        "Layers to load when running (i.e. -l window,workspace). Defaults are window,workspace and editor.", "string");
    CommandLineParser::Option* layerConfigOption = m_parser->addOption({"--layer-config"},
        "Custom configuration for layers. Each layer property is configured via a dot notation. "
        "(i.e. workspace.monitor=monitor.qml;)", "string");

    CommandLineParser::Option* layersRun = m_parser->addFlag({"--run"},
        "Run project in console mode. This is equivalent to --layers base");
    CommandLineParser::Option* layersWindow = m_parser->addFlag({"--window"},
        "Run project in window mode, do not load any workspace. This is equivalent to --layers window");
    CommandLineParser::Option* globalScript = m_parser->addFlag({"--global"},
        "Run project by looking its path up in the global packages folder.");

    CommandLineParser::Option* compile = m_parser->addFlag({"--compile"},
        "Compile an lv file.");
    CommandLineParser::Option* compileConfigOption = m_parser->addOption({"--compile-config"},
        "Custom configuration for compiler options. Each property is configured via a key value pair. "
        "(i.e. baseComponent=CustomElement;)", "string");
    CommandLineParser::Option* compileConfigFileOption = m_parser->addOption({"--compile-config-file"},
        "Custom configuration file for compiler options.", "path");

    m_parser->parse(argc, argv);

    QString layersValue = QString::fromStdString(m_parser->value(layers));
    if ( !layersValue.isEmpty() )
        m_layers = layersValue.split(',');

    if ( m_parser->isSet(layersRun) ){
        if ( !m_layers.isEmpty() )
            THROW_EXCEPTION(Exception, "Both --run and other layer options have been set.", lv::Exception::toCode("Init"));
        m_layers = QStringList() << "base";
    }
    if ( m_parser->isSet(layersWindow) ){
        if ( !m_layers.isEmpty() )
            THROW_EXCEPTION(Exception, "Both --window and other layer options have been set.", lv::Exception::toCode("Init"));
        m_layers = QStringList() << "window";
    }
    if ( m_parser->isSet(globalScript) ){
        m_globalScript = true;
    }
    if ( m_parser->isSet(m_parser->helpOption()) ){
        m_command = LivekeysArguments::Help;
    }
    if ( m_parser->isSet(m_parser->versionOption() ) ){
        m_command = LivekeysArguments::Version;
    }
    if ( m_parser->isSet(compile)){
        m_command = LivekeysArguments::Compile;
    }

    if ( m_parser->isSet(logConfigFileOption) ){
        m_logConfiguration = MLNode(MLNode::Type::Object);
        QFile lcf(QString::fromStdString(m_parser->value(logConfigFileOption)));
        if ( !lcf.open(QIODevice::ReadOnly) )
            THROW_EXCEPTION(lv::Exception, "Failed to open log configuration file: " + lcf.fileName().toStdString(), Exception::toCode("Init"));

        ml::fromJson(lcf.readAll(), m_logConfiguration);
    } else if ( m_parser->isSet(logConfigOption) ){
        // i.e: level=error; global:level=error; global:prefix=%p

        m_logConfiguration = MLNode(MLNode::Type::Object);

        // separate assignments, and check for optional configuration name
        QStringList cf = QString::fromStdString(m_parser->value(logConfigOption)).split(";");
        for( auto it = cf.begin(); it != cf.end(); ++it ){
            QString& cfs = *it;
            if ( cfs.isEmpty() )
                continue;
            int apos = cfs.indexOf('=');
            if ( apos == -1 )
                THROW_EXCEPTION(lv::Exception, "Failed to parse configuration segment: " + cfs.toStdString(), Exception::toCode("Init"));

            QString configurationName = "global";
            QString configurationKey = "";
            QString configurationValue = "";
            int tpos = cfs.indexOf(':');

            if ( tpos != -1 && tpos < apos){
                configurationName  = cfs.mid(0, tpos);
                configurationKey   = cfs.mid(tpos + 1, apos - tpos - 1);
                configurationValue = cfs.mid(apos + 1);
            } else {
                configurationKey   = cfs.mid(0, apos);
                configurationValue = cfs.mid(apos + 1);
            }

            if ( configurationName.isEmpty() || configurationKey.isEmpty() || configurationValue.isEmpty() )
                THROW_EXCEPTION(lv::Exception, "Failed to parse configuration segment: " + cfs.toStdString(), Exception::toCode("Init"));

            MLNode::StringType cfgname = configurationName.toStdString();
            MLNode::StringType cfgkey  = configurationKey.toStdString();
            if ( !m_logConfiguration.hasKey(configurationName.toStdString()) )
                m_logConfiguration[cfgname] = MLNode(MLNode::Type::Object);

            if ( configurationKey == "logDaily" || configurationKey == "toView" || configurationKey == "toConsole" ){
                m_logConfiguration[cfgname][cfgkey] = configurationValue.toLower() == "true" ? true : false;
            } else {
                m_logConfiguration[cfgname][cfgkey] = configurationValue.toStdString();
            }
        }


    } else {
        m_logConfiguration = MLNode(MLNode::Type::Object);
        m_logConfiguration["global"] = MLNode(MLNode::Type::Object);
        if ( m_parser->isSet(logPrefixOption) ){
            m_logConfiguration["global"]["prefix"] = m_parser->value(logPrefixOption);
        }
        if ( m_parser->isSet(logDailyOption) ){
            m_logConfiguration["global"]["daily"] = true;
        }
        if ( m_parser->isSet(logToNoViewOption) ) {
            m_logConfiguration["global"]["toView"] = false;
        }
        if ( m_parser->isSet(logToNetworkOption) ){
            m_logConfiguration["global"]["toNetwork"] = m_parser->value(logToNetworkOption);
        }
        if ( m_parser->isSet(logLevelOption) ){
            m_logConfiguration["global"]["level"] = m_parser->value(logLevelOption);
        }
        if ( m_parser->isSet(logToConsoleOption) ){
            m_logConfiguration["global"]["toConsole"] = true;
        }
        if ( m_parser->isSet(logFileOption) ){
            m_logConfiguration["global"]["file"] = m_parser->value(logFileOption);
        }
    }


    if ( m_parser->isSet(compileConfigOption) ){
        m_compileConfiguration = MLNode(MLNode::Type::Object);

        // separate assignments, and check for optional configuration name
        QStringList cf = QString::fromStdString(m_parser->value(compileConfigOption)).split(";");
        for( auto it = cf.begin(); it != cf.end(); ++it ){
            QString& cfs = *it;
            if ( cfs.isEmpty() )
                continue;
            int apos = cfs.indexOf('=');
            if ( apos == -1 )
                THROW_EXCEPTION(lv::Exception, "Failed to parse configuration segment: " + cfs.toStdString(), Exception::toCode("Init"));

            QString configurationKey = "";
            QString configurationValue = "";

            configurationKey   = cfs.mid(0, apos);
            configurationValue = cfs.mid(apos + 1);

            if ( configurationKey.isEmpty() || configurationValue.isEmpty() )
                THROW_EXCEPTION(lv::Exception, "Failed to parse configuration segment: " + cfs.toStdString(), Exception::toCode("Init"));

            MLNode::StringType cfgkey  = configurationKey.toStdString();
            m_compileConfiguration[cfgkey] = configurationValue.toStdString();
        }
    }
    if ( m_parser->isSet(compileConfigFileOption) ){
        m_compileConfiguration = MLNode();
        QFile lcf(QString::fromStdString(m_parser->value(compileConfigFileOption)));
        if ( !lcf.open(QIODevice::ReadOnly) )
            THROW_EXCEPTION(lv::Exception, "Failed to open log configuration file: " + lcf.fileName().toStdString(), Exception::toCode("Init"));

        QByteArray content = lcf.readAll();
        ml::fromJson(content, m_compileConfiguration);
    }

    if ( m_parser->isSet(layerConfigOption) ){
        // i.e: workspace.monitor=monitor.qml;editor.highlighter=disabled
        m_layerConfiguration = MLNode(MLNode::Type::Object);

        // separate assignments, and check for optional configuration name
        QStringList cf = QString::fromStdString(m_parser->value(layerConfigOption)).split(";");
        for( auto it = cf.begin(); it != cf.end(); ++it ){
            QString& cfs = *it;
            if ( cfs.isEmpty() )
                continue;
            int apos = cfs.indexOf('=');
            if ( apos == -1 )
                THROW_EXCEPTION(lv::Exception, Utf8("Failed to parse layer configuration segment: %").format(cfs), Exception::toCode("Init"));
            int tpos = cfs.indexOf('.');
            if ( tpos == -1 || tpos > apos ){
                THROW_EXCEPTION(lv::Exception, Utf8("Failed to parse layer configuration segment: %").format(cfs), Exception::toCode("Init"));
            }

            QString configurationName  = cfs.mid(0, tpos);
            QString configurationKey   = cfs.mid(tpos + 1, apos - tpos - 1);
            QString configurationValue = cfs.mid(apos + 1);

            if ( configurationName.isEmpty() || configurationKey.isEmpty() || configurationValue.isEmpty() )
                THROW_EXCEPTION(lv::Exception, Utf8("Failed to parse configuration segment: %").format(cfs), Exception::toCode("Init"));

            MLNode::StringType cfgname = configurationName.toStdString();
            MLNode::StringType cfgkey  = configurationKey.toStdString();
            // setup configuration for layer if it hasn't been set yet
            if ( !m_layerConfiguration.hasKey(cfgname) )
                m_layerConfiguration[cfgname] = MLNode(MLNode::Type::Object);
            m_layerConfiguration[cfgname][cfgkey] = configurationValue.toStdString();
        }
    }
}

const MLNode &LivekeysArguments::layerConfiguration() const{
    return m_layerConfiguration;
}

MLNode LivekeysArguments::layerConfigurationFor(const std::string &layerName) const{
    if ( m_layerConfiguration.type() == MLNode::Object && m_layerConfiguration.hasKey(layerName) ){
        return m_layerConfiguration[layerName];
    }
    return MLNode();
}

const MLNode &LivekeysArguments::logConfiguration() const{
    return m_logConfiguration;
}

const MLNode &LivekeysArguments::compileConfiguration() const{
    return m_compileConfiguration;
}

bool LivekeysArguments::globalFlag() const{
    return m_globalScript;
}

std::string LivekeysArguments::helpString() const{
    return m_parser->helpString();
}

const QStringList &LivekeysArguments::layers() const{
    return m_layers;
}

const std::vector<std::string> &LivekeysArguments::scriptArguments() const{
    return m_parser->scriptArguments();
}

const std::vector<std::string> &LivekeysArguments::arguments() const{
    return m_parser->arguments();
}

const std::string &LivekeysArguments::script() const{
    return m_parser->script();
}

}// namespace
