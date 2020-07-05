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

#include "qmlfilereader.h"
#include "live/applicationcontext.h"
#include "live/exception.h"
#include "live/viewengine.h"
#include "live/viewcontext.h"
#include <QFile>
#include <QFileSystemWatcher>

namespace lv{

QmlFileReader::QmlFileReader(QObject *parent)
    : QObject(parent)
    , m_componentComplete(false)
    , m_watcher(nullptr)
{
}

QmlFileReader::~QmlFileReader(){
    if ( m_watcher ){
        disconnect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(systemFileChanged(QString)));
        m_watcher->deleteLater();
    }
}

void QmlFileReader::setSource(QString source){
    if (m_source == source)
        return;
    if ( m_watcher ){
        if ( !m_source.isEmpty() )
            m_watcher->removePath(m_source);
        if ( !source.isEmpty() )
            m_watcher->addPath(source);
    }

    m_source = source;
    emit sourceChanged(source);

    resync();
}

void QmlFileReader::setMonitor(bool monitor){
    if ( monitor ){
        if ( !m_watcher ){
            m_watcher = new QFileSystemWatcher;
            if ( !m_source.isEmpty() )
                m_watcher->addPath(m_source);
            connect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(systemFileChanged(QString)));
        }
    } else if ( m_watcher ){
        disconnect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(systemFileChanged(QString)));
        m_watcher->deleteLater();
        m_watcher = 0;
    }

    emit monitorChanged();
}

void QmlFileReader::systemFileChanged(const QString &){
    resync();
}

void QmlFileReader::resync(){
    if ( m_componentComplete && m_source != "" ){
        QFile fileInput(m_source);
        if ( !fileInput.open(QIODevice::ReadOnly ) ){
            lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Cannot open file: " + m_source.toStdString(), 0);
            lv::ViewContext::instance().engine()->throwError(&e);
            return;
        }
        m_data = fileInput.readAll();
        emit dataChanged(m_data);
    }
}

}// namespace
