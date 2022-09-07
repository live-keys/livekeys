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

#include "loglistenersocket.h"
#include "live/applicationcontext.h"
#include "live/visuallogqt.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include "live/mlnodetojson.h"
#include "live/viewcontext.h"
#include "live/datetime.h"

#include <QMetaType>
#include <QMetaObject>
#include <QHostAddress>
#include <QTcpSocket>

namespace lv{

// LogListenerSocket::ObjectMessageInfo
// ---------------------------------------------------------------------

/// \private
class LogListenerSocket::ObjectMessageInfo{
public:
    lv::VisualLog::MessageInfo::Level level;
    int line;
    QString address;
    QString functionName;
    QByteArray typeName;
    DateTime stamp;
};

// LogListenerSocket
// ---------------------------------------------------------------------

LogListenerSocket::LogListenerSocket(QTcpSocket* socket, QObject *parent)
    : QObject(parent)
    , m_socket(socket)
    , m_expectObject(0)
{
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this,     SLOT(tcpError(QAbstractSocket::SocketError)));
    connect(m_socket, SIGNAL(readyRead()),
            this,     SLOT(tcpRead()));
    m_socket->setSocketOption(QAbstractSocket::KeepAliveOption, true);
    m_address = m_socket->peerAddress().toString();
}

LogListenerSocket::~LogListenerSocket(){
    if ( m_socket->state() == QTcpSocket::ConnectedState ){
        m_socket->disconnectFromHost();
        m_socket->waitForDisconnected(5000);
    }
    delete m_socket;
}

void LogListenerSocket::tcpRead(){
    QByteArray received = m_socket->readAll();
    int lastCut = 0;
    for ( int i = 0; i < received.size(); ++i ){
        if ( received[i] == '\n' ){
            logLine(m_buffer + received.mid(lastCut, i - lastCut));
            lastCut = i + 1;
            m_buffer.clear();
        }
    }
    if ( lastCut != received.size() )
        m_buffer = received.mid(lastCut);
}

void LogListenerSocket::tcpError(QAbstractSocket::SocketError){
    lv::Exception e = CREATE_EXCEPTION(
        lv::Exception, "Log listener socket error: " + m_socket->errorString().toStdString(), 0
    );
    lv::ViewContext::instance().engine()->throwError(&e);
}

bool LogListenerSocket::isPrefix(
        const QByteArray &buffer,
        int dateIndex,
        int &levelIndex,
        int &functionIndex,
        int &lineIndex,
        int &separatorIndex)
{
    int numberIndexes[] = {0, 1, 2, 3, 5, 6, 8, 9, 11, 12, 14, 15, 17, 18, 20, 21, 22};
    for ( int i = 0; i < 17; ++i ){
        if ( buffer[numberIndexes[i + dateIndex]] < '0' || buffer[numberIndexes[i + dateIndex]] > '9' ){
            return false;
        }
    }

    if ( buffer[dateIndex + 4]  != '-' || buffer[dateIndex + 7]  != '-' || buffer[dateIndex + 10] != ' ' ||
         buffer[dateIndex + 13] != ':' || buffer[dateIndex + 16] != ':' || buffer[dateIndex + 19] != '.' ||
         buffer[dateIndex + 23] != ' ')
    {
        return false;
    }

    levelIndex = dateIndex + 24;

    functionIndex = buffer.indexOf(' ', levelIndex);
    if ( functionIndex == -1 )
        return false;
    functionIndex++;

    lineIndex = buffer.indexOf('@', functionIndex);
    if ( lineIndex == -1 )
        return false;
    lineIndex++;

    separatorIndex = buffer.indexOf(": ", lineIndex);
    if ( separatorIndex == -1 )
        return false;

    return true;
}

int LogListenerSocket::isIp(const QByteArray &buffer){
    for ( int i = 0; i < buffer.size(); ++i ){
        if ( buffer[i] == '>' && i < buffer.size() - 1 && buffer[i + 1] == ' ' && i >= 6)
            return i;
        if ( buffer[i] < 0 || buffer[i] > 9 || buffer[i] != '.')
            return -1;
    }
    return -1;
}

void LogListenerSocket::logLine(const QByteArray &buffer){
    if ( m_expectObject ){
        //TODO
//        lv::QmlMetaExtension::Ptr ti = lv::ViewContext::instance().engine()->typeInfo(m_expectObject->typeName);

//        lv::VisualLog vl(m_expectObject->level);
//        vl.at(m_expectObject->address.toStdString(), "", m_expectObject->line, m_expectObject->functionName.toStdString());
//        vl.overrideStamp(m_expectObject->stamp);

//        if ( !ti.isNull() && ti->isSerializable() && ti->isLoggable() ){
//            try{
//                lv::MLNode node;
//                lv::ml::fromJson(buffer, node);
//                QObject* object = ti->deserialize(lv::ViewContext::instance().engine(), node);
//                ti->log(vl, object);
//            } catch ( lv::Exception& e ){
//                lv::ViewContext::instance().engine()->throwError(&e, this);
//            }

//        } else {
//            vl << "[Object object]";
//        }

        delete m_expectObject;
        m_expectObject = 0;

    } else {
        if ( buffer.size() == 0 ){
            lv::VisualLog().at(m_address.toStdString(), "");
            return;
        }

        if ( buffer.size() > MinimumPrefixSize ){
            int ipEnd = isIp(buffer);
            int dateIndex = ipEnd == -1 ? 0 : ipEnd + 2;

            if ( buffer.size() > dateIndex + MinimumPrefixSize ){
                int levelIndex, functionIndex, lineIndex, separatorIndex;
                if ( isPrefix(buffer, dateIndex, levelIndex, functionIndex, lineIndex, separatorIndex) ){

                    int year    = buffer.mid(dateIndex, 4).toInt();
                    int month   = buffer.mid(dateIndex + 5, 2).toInt();
                    int day     = buffer.mid(dateIndex + 8, 2).toInt();

                    int hour    = buffer.mid(dateIndex + 11, 2).toInt();
                    int minute  = buffer.mid(dateIndex + 14, 2).toInt();
                    int second  = buffer.mid(dateIndex + 17, 2).toInt();
                    int msecond = buffer.mid(dateIndex + 20, 3).toInt();

                    DateTime stamp(year, month, day, hour, minute, second, msecond);

                    lv::VisualLog::MessageInfo::Level level = lv::VisualLog::MessageInfo::levelFromString(
                        buffer.mid(levelIndex, functionIndex - levelIndex - 1).toStdString()
                    );

                    QString functionName = buffer.mid(functionIndex, lineIndex - functionIndex - 1);
                    int line = buffer.mid(lineIndex, separatorIndex - lineIndex -1).toInt();

                    if ( buffer.size() > separatorIndex + 5 &&
                         buffer[separatorIndex + 2] == '\\' &&
                         buffer[separatorIndex + 3] == '@' )
                    {
                        m_expectObject = new LogListenerSocket::ObjectMessageInfo;
                        m_expectObject->address      = buffer.mid(dateIndex) + m_address;
                        m_expectObject->level        = level;
                        m_expectObject->functionName = functionName;
                        m_expectObject->line         = line;
                        m_expectObject->stamp        = stamp;
                        m_expectObject->typeName     = buffer.mid(separatorIndex + 4);
                    } else {
                        lv::VisualLog(level).at((buffer.mid(0, dateIndex) + m_address).toStdString(), "", line, functionName.toStdString())
                                            .overrideStamp(stamp) << buffer.mid(separatorIndex + 2);
                    }

                    return;
                }
            }
        }


        if ( buffer.size() > 3 && buffer[0] == '\\' && buffer[1] == '@' )
        {
            m_expectObject = new LogListenerSocket::ObjectMessageInfo;
            m_expectObject->stamp = DateTime().toLocal();
            m_expectObject->typeName = buffer.mid(2);
        } else {
            lv::VisualLog().at(m_address.toStdString(), "") << buffer;
        }
    }
}

} // namespace
