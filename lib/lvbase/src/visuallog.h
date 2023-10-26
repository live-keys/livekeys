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

#ifndef LVVISUALLOG_H
#define LVVISUALLOG_H

#include <iostream>
#include <sstream>
#include <ostream>
#include <functional>

#include "live/mlnode.h"

namespace lv{

class Utf8;
class DateTime;

class LV_BASE_EXPORT VisualLog{

public:
    class Configuration;
    class ConfigurationContainer;

    typedef std::function<void(int, const std::string&)> MessageHandlerFunction;

    /** Bitmasks for each type of output */
    enum Output{
        /** Console output */
        Console = 1,
        /** Error output */
        Error = 2,
        /** File output */
        File = 4,
        /** View output */
        View = 8,
        /** Extentions output */
        Extensions = 16
    };

    /**
      \class lv::VisualLog::SourceLocation
      \brief Simple structure containing relevant data about a location of the source

      Location info includes a remote, function name, line number and file name. All of these are optional.
      \ingroup lvbase
     */
    class SourceLocation{
    public:
        /** Default contructor */
        SourceLocation(const std::string& file, int line, const std::string& fileName);
        /** Contructor with a remote */
        SourceLocation(const std::string& remote, const std::string& file, int line, const std::string& functionName);

        /** Remote */
        std::string remote;
        /** Source file */
        std::string file;
        /** Source line */
        int         line;
        /** Function name */
        std::string functionName;
    };

    class LV_BASE_EXPORT MessageInfo{

    public:
        /**
         * \brief Collection of all possible message levels, in decreasing order of importance
         */
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
        int         sourceLineNumber() const;
        std::string sourceFunctionName() const;
        const DateTime& stamp() const;
        std::string prefix(const VisualLog::Configuration* configuration) const;
        std::string tag(const VisualLog::Configuration* configuration) const;
        Level       level() const;

    private:
        MessageInfo();
        MessageInfo(Level level);
        MessageInfo(const MessageInfo&);
        MessageInfo& operator = (const MessageInfo&);

        std::string expand(const std::string& pattern) const;

        Level            m_level;
        SourceLocation*  m_location;
        mutable DateTime* m_stamp;
    };

    /**
     * \class lv::VisualLog::Transport
     * \brief Abstraction of the transport used to pass non-visual/object messages
     *
     * \ingroup lvbase
     */
    class LV_BASE_EXPORT Transport{
    public:
        virtual ~Transport(){}
        /** Declaration of the onMessage method */
        virtual void onMessage(const VisualLog::Configuration* configuration,
                               const VisualLog::MessageInfo& messageInfo,
                               const std::string& message) = 0;
        /** Declaration of the onObject method */
        virtual void onObject(const VisualLog::Configuration* configuration,
                              const VisualLog::MessageInfo& messageInfo,
                              const std::string& type,
                              const MLNode& node) = 0;
    };


    class LV_BASE_EXPORT ViewObject{
    public:
        virtual ~ViewObject(){}
    };

    /**
     * \class lv::VisualLog::ViewTransport
     * \brief Abstraction of the transport used to pass visual (view) messages
     *
     * \ingroup lvbase
     */
    class LV_BASE_EXPORT ViewTransport{
    public:
        virtual ~ViewTransport(){}
        /** Declaration of the onMessage method */
        virtual void onMessage(
            const VisualLog::Configuration* configuration,
            const VisualLog::MessageInfo& messageInfo,
            const std::string& message
        ) = 0;
        /** Declaration of the onView method */
        virtual void onView(
            const VisualLog::Configuration* configuration,
            const VisualLog::MessageInfo& messageInfo,
            const std::string& viewName,
            ViewObject* vo
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
    VisualLog& overrideStamp(const DateTime &stamp);

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

    void asObject(const std::string& type, const MLNode& value);
    template<typename LogBehavior, typename T> VisualLog& behavior(const T& value);

    static ViewTransport* model();
    static void setViewTransport(ViewTransport* model);

    static void flushConsole(const std::string& data);
    static void setInternalMessageHandler(const MessageHandlerFunction& fn);

private:
    DISABLE_COPY(VisualLog);

    void init();
    void flushFile(const std::string &data);
    void flushHandler(const std::string& data);
    std::string prefix();
    bool canLogObjects(VisualLog::Configuration* configuration);

    template<typename T> void object(MessageInfo::Level level, const T& value);

    static int removeOutputFlag(int flags, VisualLog::Output output);

    static ConfigurationContainer createDefaultConfigurations();
    static ConfigurationContainer& registeredConfigurations();
    static MessageHandlerFunction& internalMessageHandler();
    static void defaultInternalMessageHandler(int, const std::string& message);

    static ViewTransport* m_model;

    static bool m_globalConfigured;

    int                m_output;
    Configuration*     m_configuration;
    MessageInfo        m_messageInfo;
    std::stringstream* m_stream;
    bool               m_objectOutput;

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

/** \brief Log given value as object */
template<typename T> void VisualLog::object(const T &value){
    m_objectOutput = true;
    if ( canLog() )
        *this << value;
}

template<typename T> void VisualLog::object(VisualLog::MessageInfo::Level level, const T& value){
    m_messageInfo.m_level = level;
    object(value);
}


template<typename LogBehavior, typename T>
VisualLog &VisualLog::behavior(const T &value){
    // as object
    if ( canLog() && m_objectOutput && canLogObjects(m_configuration) ){
        if ( LogBehavior::hasTransport() ){
            MLNode mlvalue;
            LogBehavior::toTransport(value, mlvalue);
            ml::serialize(value, mlvalue);
            asObject(LogBehavior::typeId(value), mlvalue);
        }
    }
    // as view
    if ( canLog() && m_objectOutput && (m_output & VisualLog::View) ){
        if ( LogBehavior::hasViewObject() ){
            VisualLog::ViewObject* vo = LogBehavior::toView(value);
            std::string viewDelegate = LogBehavior::defaultViewDelegate(value);
            m_model->onView(m_configuration, m_messageInfo, viewDelegate, vo);
            m_output = removeOutputFlag(m_output, VisualLog::View);
        }
    }
    // as stream
    LogBehavior::toStream(*this, value);
    return *this;
}

/** \brief Log given value as object with level Fatal */
template<typename T> void VisualLog::f(const T &value){
    object(VisualLog::MessageInfo::Fatal, value);
}

/** \brief Log given value as object with level Error */
template<typename T> void VisualLog::e(const T &value){
    object(VisualLog::MessageInfo::Error, value);
}

/** \brief Log given value as object with level Warning */
template<typename T> void VisualLog::w(const T &value){
    object(VisualLog::MessageInfo::Warning, value);
}

/** \brief Log given value as object with level Info */
template<typename T> void VisualLog::i(const T &value){
    object(VisualLog::MessageInfo::Info, value);
}

/** \brief Log given value as object with level Debug */
template<typename T> void VisualLog::d(const T &value){
    object(VisualLog::MessageInfo::Debug, value);
}

/** \brief Log given value as object with level Verbose */
template<typename T> void VisualLog::v(const T &value){
    object(VisualLog::MessageInfo::Verbose, value);
}

/** \brief Sets the message level to Fatal */
inline VisualLog &VisualLog::f(){
    m_messageInfo.m_level = VisualLog::MessageInfo::Fatal;
    return *this;
}

/** \brief Sets the message level to Error */
inline VisualLog &VisualLog::e(){
    m_messageInfo.m_level = VisualLog::MessageInfo::Error;
    return *this;
}
/** \brief Sets the message level to Warning */
inline VisualLog &VisualLog::w(){
    m_messageInfo.m_level = VisualLog::MessageInfo::Warning;
    return *this;
}
/** \brief Sets the message level to Info */
inline VisualLog &VisualLog::i(){
    m_messageInfo.m_level = VisualLog::MessageInfo::Info;
    return *this;
}
/** \brief Sets the message level to Debug */
inline VisualLog &VisualLog::d(){
    m_messageInfo.m_level = VisualLog::MessageInfo::Debug;
    return *this;
}

/** \brief Sets the message level to Verbose */
inline VisualLog &VisualLog::v(){
    m_messageInfo.m_level = VisualLog::MessageInfo::Verbose;
    return *this;
}

/** \brief Sets the message info location */
inline VisualLog &VisualLog::at(const std::string &file, int line, const std::string &functionName){
    m_messageInfo.m_location = new VisualLog::SourceLocation(file, line, functionName);
    return *this;
}

/** \brief Sets the message info location with remote included */
inline VisualLog &VisualLog::at(const std::string &remote, const std::string &file, int line, const std::string &functionName){
    m_messageInfo.m_location = new VisualLog::SourceLocation(remote, file, line, functionName);
    return *this;
}

/**
 * \brief Returns a remote location, if it exists
 */
inline std::string VisualLog::MessageInfo::sourceRemoteLocation() const{
    return m_location ? m_location->remote : "";
}

/**
 * \brief Returns the file name of the source, if it exists
 */
inline std::string VisualLog::MessageInfo::sourceFileName() const{
    return m_location ? m_location->file : "";
}

/**
 * \brief Returns the line number, if it has been set
 */
inline int VisualLog::MessageInfo::sourceLineNumber() const{
    return m_location ? m_location->line : 0;
}

/**
 * \brief Returns the name of the source function, if it has been set
 */
inline std::string VisualLog::MessageInfo::sourceFunctionName() const{
    return m_location ? m_location->functionName : "";
}

/** \brief Stream insertion operator */
template<typename T> VisualLog& VisualLog::operator<< (const T& x){
    if ( !canLog() )
        return *this;

    *m_stream << x;

    return *this;
}

/** \brief Stream insertion operator with a std::ostream& (*f)(std::ostream&) parameter */
template<typename T> VisualLog& VisualLog::operator<< (std::ostream &(*f)(std::ostream &)){
    if ( !canLog() )
        return *this;

    std::stringstream ss;
    f(ss);
    *m_stream << ss.str().c_str();

    return *this;
}

/** \brief Stream insertion operator with a std::ostream& (*f)(std::ios&)  parameter */
template<typename T> VisualLog& VisualLog::operator<< ( std::ostream& (*f)(std::ios&) ){
    if ( !canLog() )
        return *this;

    std::stringstream ss;
    f(ss);
    *m_stream << ss.str().c_str();

    return *this;
}

/** \brief Stream insertion operator with a std::ostream (*f)(std::ios_base& ) parameter */
template<typename T> VisualLog& VisualLog::operator<< ( std::ostream (*f)(std::ios_base& ) ){
    if ( !canLog() )
        return *this;

    std::stringstream ss;
    f(ss);
    *m_stream << ss.str().c_str();

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


