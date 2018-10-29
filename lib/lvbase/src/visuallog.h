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

#ifndef LVVISUALLOG_H
#define LVVISUALLOG_H

#include <iostream>
#include <sstream>
#include <ostream>
#include <functional>

#include "live/mlnode.h"

class QDateTime;
class QVariant;

namespace lv{

class LV_BASE_EXPORT VisualLog{

public:
    enum Output{
        Console = 1,
        Error = 2,
        File = 4,
        View = 8,
        Extensions = 16
    };

    class Configuration;
    class ConfigurationContainer;

    class SourceLocation{
    public:
        SourceLocation(const std::string& file, int line, const std::string& fileName);
        SourceLocation(const std::string& remote, const std::string& file, int line, const std::string& functionName);

        std::string remote;
        std::string file;
        int         line;
        std::string functionName;
    };

    class LV_BASE_EXPORT MessageInfo{

    public:
        enum Level{
            Fatal = 0,
            Error,
            Warning,
            Info,
            Debug,
            Verbose
        };

        friend class VisualLog;

    public:
        ~MessageInfo();

        static std::string levelToString(VisualLog::MessageInfo::Level level);
        static VisualLog::MessageInfo::Level levelFromString(const std::string& str);

        std::string sourceRemoteLocation() const;
        std::string sourceFileName() const;
        int     sourceLineNumber() const;
        std::string sourceFunctionName() const;
        const QDateTime& stamp() const;
        std::string prefix(const VisualLog::Configuration* configuration) const;
        std::string tag(const VisualLog::Configuration* configuration) const;

    private:
        MessageInfo();
        MessageInfo(Level level);
        MessageInfo(const MessageInfo&);
        MessageInfo& operator = (const MessageInfo&);

        std::string expand(const std::string& pattern) const;

        Level              m_level;
        SourceLocation*    m_location;
        mutable QDateTime* m_stamp;
    };

    class LV_BASE_EXPORT Transport{
    public:
        virtual ~Transport(){}
        virtual void onMessage(const VisualLog::Configuration* configuration,
                               const VisualLog::MessageInfo& messageInfo,
                               const std::string& message) = 0;
        virtual void onObject(const VisualLog::Configuration* configuration,
                              const VisualLog::MessageInfo& messageInfo,
                              const std::string& type,
                              const MLNode& node) = 0;
    };

    class LV_BASE_EXPORT ViewTransport{
    public:
        virtual ~ViewTransport(){}
        virtual void onMessage(
            const VisualLog::Configuration* configuration,
            const VisualLog::MessageInfo& messageInfo,
            const std::string& message
        ) = 0;
        virtual void onView(
            const VisualLog::Configuration* configuration,
            const VisualLog::MessageInfo& messageInfo,
            const std::string& viewName,
            const QVariant& value
        ) = 0;
    };

public:
    VisualLog();
    VisualLog(MessageInfo::Level level);
    VisualLog(const std::string& configuration);
    VisualLog(const std::string& configuration, MessageInfo::Level level);
    ~VisualLog();

    VisualLog& at(const std::string& file, int line = 0, const std::string& functionName = "");
    VisualLog& at(const std::string& remote, const std::string& file, int line = 0, const std::string& functionName = "");
    VisualLog& overrideStamp(const QDateTime& stamp);

    template<typename T> VisualLog& operator <<( const T& x );
    template<typename T> VisualLog& operator <<( std::ostream& (*f)(std::ostream&) );
    template<typename T> VisualLog& operator <<( std::ostream& (*f)(std::ios&) );
    template<typename T> VisualLog& operator <<( std::ostream (*f)(std::ios_base& ) );

    VisualLog& f();
    VisualLog& e();
    VisualLog& w();
    VisualLog& i();
    VisualLog& d();
    VisualLog& v();

    template<typename T> void object(const T& value);
    template<typename T> void f(const T& value);
    template<typename T> void e(const T& value);
    template<typename T> void w(const T& value);
    template<typename T> void i(const T& value);
    template<typename T> void d(const T& value);
    template<typename T> void v(const T& value);


    bool canLog();
    void configure(const std::string& configuration, const MLNode &options);
    void configure(VisualLog::Configuration* configuration, const MLNode& options);
    void addTransport(const std::string& configuration, Transport* transport);
    void addTransport(VisualLog::Configuration* configuration, Transport* transport);
    void removeTransports(const std::string& configuration);
    void removeTransports(VisualLog::Configuration* configuration);

    int totalConfigurations();

    void flushLine();
    void closeFile();

    void asView(const std::string& viewPath, const QVariant& viewData);
    void asView(const std::string& viewPath, std::function<QVariant()> cloneFunction);
    template<typename T> void asObject(const std::string& type, const T& value);
    void asObject(const std::string& type, const MLNode& value);


    static ViewTransport* model();
    static void setViewTransport(ViewTransport* model);

    static void flushConsole(const std::string& data);

private:
    // disable copy
    VisualLog(const VisualLog&);
    VisualLog& operator = (const VisualLog&);

    void init();
    void flushFile(const std::string &data);
    void flushHandler(const std::string& data);
    std::string prefix();
    bool canLogObjects(VisualLog::Configuration* configuration);

    template<typename T> void object(MessageInfo::Level level, const T& value);

    static int removeOutputFlag(int flags, VisualLog::Output output);

    static ConfigurationContainer createDefaultConfigurations();
    static ConfigurationContainer m_registeredConfigurations;

    static ViewTransport* m_model;

    static bool m_globalConfigured;

    int               m_output;
    Configuration*    m_configuration;
    MessageInfo       m_messageInfo;
    std::stringstream m_stream;
    bool              m_objectOutput;

};

// VisualLog::SourceLocation
// ---------------------------------------------------------------------

inline VisualLog::SourceLocation::SourceLocation(const std::string &pfile, int pline, const std::string &pFunctionName)
    : file(pfile)
    , line(pline)
    , functionName(pFunctionName)
{
}

inline VisualLog::SourceLocation::SourceLocation(
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

template<typename T> void VisualLog::object(const T &value){
    m_objectOutput = true;
    if ( canLog() )
        *this << value;
}

template<typename T> void VisualLog::object(VisualLog::MessageInfo::Level level, const T& value){
    m_messageInfo.m_level = level;
    object(value);
}

template<typename T> void VisualLog::asObject(const std::string &type, const T &value){
    if ( canLog() && m_objectOutput && canLogObjects(m_configuration) ){
        MLNode mlvalue;
        ml::serialize(value, mlvalue);
        asObject(type, mlvalue);
    }
}

template<typename T> void VisualLog::f(const T &value){
    object(VisualLog::MessageInfo::Fatal, value);
}
template<typename T> void VisualLog::e(const T &value){
    object(VisualLog::MessageInfo::Error, value);
}
template<typename T> void VisualLog::w(const T &value){
    object(VisualLog::MessageInfo::Warning, value);
}
template<typename T> void VisualLog::i(const T &value){
    object(VisualLog::MessageInfo::Info, value);
}
template<typename T> void VisualLog::d(const T &value){
    object(VisualLog::MessageInfo::Debug, value);
}
template<typename T> void VisualLog::v(const T &value){
    object(VisualLog::MessageInfo::Verbose, value);
}

inline VisualLog &VisualLog::f(){
    m_messageInfo.m_level = VisualLog::MessageInfo::Fatal;
    return *this;
}

inline VisualLog &VisualLog::e(){
    m_messageInfo.m_level = VisualLog::MessageInfo::Error;
    return *this;
}

inline VisualLog &VisualLog::w(){
    m_messageInfo.m_level = VisualLog::MessageInfo::Warning;
    return *this;
}

inline VisualLog &VisualLog::i(){
    m_messageInfo.m_level = VisualLog::MessageInfo::Info;
    return *this;
}

inline VisualLog &VisualLog::d(){
    m_messageInfo.m_level = VisualLog::MessageInfo::Debug;
    return *this;
}

inline VisualLog &VisualLog::v(){
    m_messageInfo.m_level = VisualLog::MessageInfo::Verbose;
    return *this;
}

inline VisualLog &VisualLog::at(const std::string &file, int line, const std::string &functionName){
    m_messageInfo.m_location = new VisualLog::SourceLocation(file, line, functionName);
    return *this;
}

inline VisualLog &VisualLog::at(const std::string &remote, const std::string &file, int line, const std::string &functionName){
    m_messageInfo.m_location = new VisualLog::SourceLocation(remote, file, line, functionName);
    return *this;
}

inline std::string VisualLog::MessageInfo::sourceRemoteLocation() const{
    return m_location ? m_location->remote : "";
}

inline std::string VisualLog::MessageInfo::sourceFileName() const{
    return m_location ? m_location->file : "";
}

inline int VisualLog::MessageInfo::sourceLineNumber() const{
    return m_location ? m_location->line : 0;
}

inline std::string VisualLog::MessageInfo::sourceFunctionName() const{
    return m_location ? m_location->functionName : "";
}

template<typename T> VisualLog& VisualLog::operator<< (const T& x){
    if ( !canLog() )
        return *this;

    m_stream << x;

    return *this;
}

template<typename T> VisualLog& VisualLog::operator<< (std::ostream &(*f)(std::ostream &)){
    if ( !canLog() )
        return *this;

    std::stringstream ss;
    f(ss);
    m_stream << ss.str().c_str();

    return *this;
}

template<typename T> VisualLog& VisualLog::operator<< ( std::ostream& (*f)(std::ios&) ){
    if ( !canLog() )
        return *this;

    std::stringstream ss;
    f(ss);
    m_stream << ss.str().c_str();

    return *this;
}

template<typename T> VisualLog& VisualLog::operator<< ( std::ostream (*f)(std::ios_base& ) ){
    if ( !canLog() )
        return *this;

    std::stringstream ss;
    f(ss);
    m_stream << ss.str().c_str();

    return *this;
}

}// namespace

#ifndef VLOG_NO_MACROS

#ifndef vlog
#define vlog(...) lv::VisualLog(__VA_ARGS__).at(__FILE__, __LINE__, __FUNCTION__)
#endif // vlog

#ifndef vlog_debug
#ifdef VLOG_DEBUG_BUILD
#define vlog_debug(_configuration, _message) lv::VisualLog(_configuration).at(__FILE__, __LINE__, __FUNCTION__).v() << (_message)
#else
#define vlog_debug(_configuration, _message)
#endif // VLOG_DEBUG_BUILD
#endif // vlog_debug

#endif // VLOG_NO_MACROS


#endif // LVVISUALLOG_H


