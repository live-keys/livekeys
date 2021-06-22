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
#include "live/qmlworkerpool.h"
#include "live/qmlerror.h"

#include "opencv2/highgui.hpp"

#include <QFileSystemWatcher>

namespace{

class LoadImageTask : public lv::QmlWorkerPool::Task{

public:
    LoadImageTask(const std::string& source, int isColor, QImageFile* listener)
        : m_source(source)
        , m_isColor(isColor)
        , m_listener(listener)
        , m_error(nullptr)
    {
    }

    ~LoadImageTask(){
        delete m_error;
    }

    void run(lv::QmlWorkerPool::Thread*){
        try {
            m_result = cv::imread(m_source, m_isColor);
            if ( m_result.empty() )
                m_error = new lv::Exception(CREATE_EXCEPTION(lv::Exception, "Cannot open file: " + m_source, 0));

        } catch (cv::Exception& e) {
            m_error = new lv::Exception(lv::Exception::create<lv::Exception>(
                e.what(), e.code, e.file, e.line, e.func, lv::StackTrace::Ptr(nullptr)
            ));
        } catch ( ... ){
            m_error = new lv::Exception("Unknown exception caught in ImageFile.", lv::Exception::toCode("Unknown"));
        }

        setAutoDelete(false);
        QCoreApplication::postEvent(m_listener, new lv::QmlWorkerPool::TaskReadyEvent(this));
    }

    const cv::Mat& result() const{
        return m_result;
    }

    lv::Exception* error() const{
        return m_error;
    }

private:
    std::string     m_source;
    int             m_isColor;
    cv::Mat         m_result;
    QImageFile*     m_listener;
    lv::Exception * m_error;
};

}// namespace

QImageFile::QImageFile(QQuickItem *parent)
    : QMatDisplay(parent)
    , m_iscolor(CV_LOAD_IMAGE_COLOR)
    , m_monitor(false)
    , m_worker(nullptr)
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
        m_watcher = nullptr;
    }

    emit monitorChanged();
}

bool QImageFile::event(QEvent *ev){
    lv::QmlWorkerPool::TaskReadyEvent* tr = dynamic_cast<lv::QmlWorkerPool::TaskReadyEvent*>(ev);
    if (!tr)
        return QQuickItem::event(ev);

    auto ftask = static_cast<LoadImageTask*>(tr->task());
    if ( ftask->error() ){
        lv::QmlError qe(lv::ViewContext::instance().engine(), *ftask->error(), this);
        qe.jsThrow();
        delete ftask;
        return true;
    }

    QMat* loose = output();
    lv::Shared::ownJs(loose);

    QMat* newOutput = new QMat;
    *newOutput->internalPtr() = ftask->result();

    setOutput(newOutput);

    setImplicitWidth(output()->internalPtr()->size().width);
    setImplicitHeight(output()->internalPtr()->size().height);
    emit outputChanged();
    update();

    delete ftask;
    return true;

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

        if ( m_worker ){
            auto task = new LoadImageTask(m_source.toStdString(), m_iscolor, this);
            m_worker->start(task);
        } else {
            cv::Mat temp = cv::imread(m_source.toStdString(), m_iscolor);
            if ( temp.empty() ){
                lv::Exception e = CREATE_EXCEPTION(lv::Exception, "Cannot open file: " + m_source.toStdString(), 0);
                lv::ViewContext::instance().engine()->throwError(&e);
                return;
            }

            QMat* loose = output();
            lv::Shared::ownJs(loose);

            QMat* newOutput = new QMat;
            *newOutput->internalPtr() = temp;

            setOutput(newOutput);

            setImplicitWidth(output()->internalPtr()->size().width);
            setImplicitHeight(output()->internalPtr()->size().height);
            emit outputChanged();
            update();
        }
    }
}
