/****************************************************************************
**
** Copyright (C) 2014-2016 Dinu SV.
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

#include "qlivecvlog.h"
#include <QDir>
#include <QTextStream>

QLiveCVLog::QLiveCVLog(QObject *parent)
    : QObject(parent)
    , m_logFile(0)
{
}

QLiveCVLog::~QLiveCVLog(){
    delete m_logFile;
}

void QLiveCVLog::logMessage(QtMsgType type, const QMessageLogContext&, const QString& msg){
    m_logMutex.lock();
    switch (type){
    case QtDebugMsg:
        if ( isFileLogEnabled() )
            m_textStream << "Debug    : " << msg << "\n";
        m_data.append(msg + "<br/>");
        break;
    case QtWarningMsg:
        if ( isFileLogEnabled() )
            m_textStream << "Warning  : " << msg << "\n";
        m_data.append("<span style=\"color : #ffff00;\">" + msg + "</span><br/>");
        break;
    case QtCriticalMsg:
        if ( isFileLogEnabled() )
            m_textStream << "Critical : " << msg;
        m_data.append("<span style=\"color : #cc3333;\">" + msg + "</span><br/>");
        break;
    case QtFatalMsg:
        if ( isFileLogEnabled() )
            m_textStream << "Fatal    : " << msg;
        m_data.append("<span style=\"color : #ff0000;\">" + msg + "</span><br/>");
        break;
    }
    m_logMutex.unlock();
    emit dataChanged();
}

void QLiveCVLog::logFunction(QtMsgType type, const QMessageLogContext& ctx, const QString& msg){
    instance().logMessage(type, ctx, msg);
}

void QLiveCVLog::enableFileLog(const QString &logFilePath){
    m_logFilePath = logFilePath;
    if ( m_logFilePath == "" )
        m_logFilePath = QDir::currentPath() + "/livecv.log";

    if ( m_logFile ){
        m_textStream.setDevice(0);
        delete m_logFile;
    }

    m_logFile = new QFile(m_logFilePath);
    if ( !m_logFile->open(QIODevice::WriteOnly | QIODevice::Text ) ){
        setData("Cannot open log file for writing. Messages will show only in the log window.");
        m_logFilePath = "";
    } else {
        m_textStream.setDevice(m_logFile);
    }
}
