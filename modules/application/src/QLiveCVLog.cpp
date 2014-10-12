#include "QLiveCVLog.hpp"
#include <QDir>
#include <QTextStream>

QLiveCVLog* QLiveCVLog::m_instance = 0;

QLiveCVLog::QLiveCVLog(QObject *parent)
    : QObject(parent)
    , m_logFilePath(QDir::currentPath() + "/livecv.log")
    , m_logFile(m_logFilePath)
{
    if ( !m_logFile.open(QIODevice::WriteOnly | QIODevice::Text ) )
        setData("Cannot open log file for writing. Messages will show only in the log window.");
    else
        m_textStream.setDevice(&m_logFile);
}

QLiveCVLog::~QLiveCVLog(){
    if ( m_logFile.isOpen() )
        m_logFile.close();
}

void QLiveCVLog::logMessage(QtMsgType type, const QMessageLogContext&, const QString& msg){
    m_logMutex.lock();
    switch (type){
    case QtDebugMsg:
        if ( m_logFile.isOpen() )
            m_textStream << "Debug    : " << msg << "\n";
        m_data.append(msg + "<br/>");
        break;
    case QtWarningMsg:
        if ( m_logFile.isOpen() )
            m_textStream << "Warning  : " << msg << "\n";
        m_data.append("<span style=\"color : #ffff00;\">" + msg + "</span><br/>");
        break;
    case QtCriticalMsg:
        if ( m_logFile.isOpen() )
            m_textStream << "Critical : " << msg;
        m_data.append("<span style=\"color : #cc3333;\">" + msg + "</span><br/>");
        break;
    case QtFatalMsg:
        if ( m_logFile.isOpen() )
            m_textStream << "Fatal    : " << msg;
        m_data.append("<span style=\"color : #ff0000;\">" + msg + "</span><br/>");
        break;
    }
    m_logMutex.unlock();
    emit dataChanged();
}

void QLiveCVLog::logFunction(QtMsgType type, const QMessageLogContext& ctx, const QString& msg){
    instance()->logMessage(type, ctx, msg);
}
