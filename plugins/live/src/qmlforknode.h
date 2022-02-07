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

#ifndef LVQMLFORKNODE_H
#define LVQMLFORKNODE_H

#include "live/linecapture.h"
#include "live/exception.h"

#include <QObject>
#include <QQmlParserStatus>
#include <QQmlPropertyMap>

class QTcpSocket;
class QQmlComponent;
class QQmlContext;

namespace lv{

class SharedMemoryReadWorker;
class SharedMemoryWriteWorker;
class RemoteLineResponse;

class QmlForkNode : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

public:
    explicit QmlForkNode(QObject *parent = 0);
    ~QmlForkNode();

    void responseValueChanged(const QString& key, const QVariant& value);
    void sendError(const QByteArray& type, Exception::Code code, const QString& message);

protected:
    void componentComplete();
    void classBegin(){}

signals:
    void error(const QByteArray& type, Exception::Code code, const QString& message);

public slots:
    void onSharedMemoryReadStatusChanged(int status);
    void onSharedMemoryMessage(const QByteArray& message, int type, int id);
    void onSharedMemoryWriteStatusChanged(int status);

private:
    SharedMemoryReadWorker*  m_readSocket;
    SharedMemoryWriteWorker* m_writeSocket;

    QQmlPropertyMap*    m_post;
    RemoteLineResponse* m_response;
    QQmlComponent*      m_component;
    QQmlContext*        m_componentContext;
    QObject*            m_sourceItem;
};

} // namespace

#endif // LVQMLFORKNODE_H
