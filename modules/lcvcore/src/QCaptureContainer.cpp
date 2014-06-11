/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/

#include "QCaptureContainer.hpp"
#include "QCamCaptureThread.hpp"
#include "QVideoCaptureThread.hpp"

QCaptureContainer* QCaptureContainer::m_instance = 0;

QCaptureContainer::QCaptureContainer(QObject *parent) :
    QObject(parent)
{
}

QVideoCaptureThread *QCaptureContainer::captureThread(const QString &file)
{
    for( QList<QVideoCaptureThread*>::iterator it = m_videoCaptureThreads.begin(); it != m_videoCaptureThreads.end(); ++it ){
        if ( (*it)->file() == file )
            return *it;
    }
    QVideoCaptureThread* thread = new QVideoCaptureThread(file);
    m_videoCaptureThreads.append(thread);
    return thread;
}


QCaptureContainer::~QCaptureContainer(){
}
