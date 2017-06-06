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

#ifndef QLIVECVLOG_H
#define QLIVECVLOG_H

#include <QObject>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QMutex>

class QLiveCVLog : public QObject{

    Q_OBJECT
    Q_PROPERTY(QString data READ data WRITE setData NOTIFY dataChanged)

public:
    explicit QLiveCVLog(QObject *parent = 0);
    ~QLiveCVLog();

    const QString data() const;
    void setData(const QString& data);

    static QLiveCVLog& instance();

    static void logFunction(QtMsgType type, const QMessageLogContext& ctx, const QString& msg);

    void enableFileLog(const QString& logPath = "");
    void disableFileLog();
    bool isFileLogEnabled() const;

signals:
    void dataChanged();

public slots:
    void logMessage(QtMsgType type, const QMessageLogContext& ctx, QString msg);

private:
    QString     m_data;
    QString     m_logFilePath;
    QFile*      m_logFile;
    QTextStream m_textStream;
    QMutex      m_logMutex;

};

inline const QString QLiveCVLog::data() const{
    return m_data;
}

inline void QLiveCVLog::setData(const QString& data){
    if ( m_data != data ){
        m_data = data;
        emit dataChanged();
    }
}

inline QLiveCVLog &QLiveCVLog::instance(){
    static QLiveCVLog instance;
    return instance;
}

inline bool QLiveCVLog::isFileLogEnabled() const{
    return !m_logFilePath.isEmpty();
}

#endif // QLIVECVLOG_H

