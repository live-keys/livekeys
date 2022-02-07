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

#ifndef LVQMLFORK_H
#define LVQMLFORK_H

#include "remotecontainer.h"
#include "live/sharedmemoryreadworker.h"
#include "live/sharedmemorywriteworker.h"

#include <QObject>
#include <QQmlParserStatus>

class QProcess;

namespace lv{

class QmlFork : public RemoteContainer{

    Q_OBJECT
    Q_PROPERTY(QStringList layers READ layers WRITE setLayers NOTIFY layersChanged)

public:
    explicit QmlFork(QObject *parent = 0);
    ~QmlFork();

    const QStringList& layers() const;
    void setLayers(const QStringList& layers);

    void sendError(const QByteArray& type, int code, const QString& message) Q_DECL_OVERRIDE;
    void sendBuild(const QByteArray& buildData) Q_DECL_OVERRIDE;
    void sendInput(const MLNode& input) Q_DECL_OVERRIDE;

    void onMessage(std::function<void(const LineMessage&, void* data)> handler, void* handlerData = 0) Q_DECL_OVERRIDE;
    void onError(std::function<void(int, const std::string&)> handler) Q_DECL_OVERRIDE;
    bool isReady() const Q_DECL_OVERRIDE;

signals:
    void layersChanged();

public slots:
    void onSharedMemoryReadStatusChanged(int status);
    void onSharedMemoryMessage(const QByteArray& message, int type, int id);
    void onSharedMemoryWriteStatusChanged(int status);

protected:
    void componentComplete() override;

private:
    std::function<void(const LineMessage&, void*)> m_handler;
    std::function<void(int, const std::string&)> m_errorHandler;
    void* m_handlerData;

    QStringList m_layers;

    QProcess*   m_fork;
    bool        m_forkready;
    SharedMemoryReadWorker*  m_readSocket;
    SharedMemoryWriteWorker* m_writeSocket;
};

inline const QStringList& QmlFork::layers() const{
    return m_layers;
}

inline void QmlFork::setLayers(const QStringList& layers){
    if (m_layers == layers)
        return;

    m_layers = layers;
    emit layersChanged();
}

inline bool QmlFork::isReady() const{
    return m_forkready;
}

} // namespace

#endif // LVQMLFORK_H
