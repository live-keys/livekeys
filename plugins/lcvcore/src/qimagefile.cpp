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

#include "qimagefile.h"
#include "live/viewengine.h"
#include "live/exception.h"
#include "live/viewcontext.h"

#include "opencv2/highgui.hpp"

#include <QFileSystemWatcher>

QImageFile::QImageFile(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_iscolor(CV_LOAD_IMAGE_COLOR)
    , m_monitor(false)
    , m_watcher(nullptr)
{
}

QImageFile::~QImageFile(){
    if ( m_watcher ){
        disconnect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(systemFileChanged()));
        m_watcher->deleteLater();
    }
}

void QImageFile::setSource(const QString &source){
    if (m_source == source)
        return;

    if ( m_watcher ){
        if ( !m_source.isEmpty() )
            m_watcher->removePath(m_source);

        if ( !source.isEmpty() )
            m_watcher->addPath(source);
    }

    m_source = source;
    emit sourceChanged();

    loadImage();
}

void QImageFile::setIscolor(int iscolor){
    if (m_iscolor != iscolor){
        m_iscolor = iscolor;
        emit iscolorChanged();
        loadImage();
    }
}

void QImageFile::setMonitor(bool monitor){
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

void QImageFile::systemFileChanged(const QString &){
    loadImage();
}

void QImageFile::open(const QString &file){
    setSource(file);
}


void QImageFile::componentComplete(){
    QQuickItem::componentComplete();
    emit init();
    loadImage();
}

void QImageFile::loadImage(){
    if ( m_source != "" && isComponentComplete() ){
        cv::Mat temp = cv::imread(m_source.toStdString(), m_iscolor);
        if ( temp.empty() ){
            lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Cannot open file: " + m_source.toStdString(), 0);
            lv::ViewContext::instance().engine()->throwError(&e);
            return;
        }

        QMat* loose = output();
        lv::Shared::ownJs(loose);

        QMat* newOutput = new QMat;
        *newOutput->cvMat() = temp;

        setOutput(newOutput);

        setImplicitWidth(output()->cvMat()->size().width);
        setImplicitHeight(output()->cvMat()->size().height);
        emit outputChanged();
        update();
    }
}
