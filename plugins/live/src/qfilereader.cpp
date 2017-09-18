#include "qfilereader.h"
#include "live/plugincontext.h"
#include "live/exception.h"
#include "live/engine.h"
#include <QFile>
#include <QFileSystemWatcher>

QFileReader::QFileReader(QObject *parent)
    : QObject(parent)
    , m_componentComplete(false)
    , m_watcher(0)
{
}

QFileReader::~QFileReader(){
    if ( m_watcher ){
        disconnect(m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(systemFileChanged(QString)));
        m_watcher->deleteLater();
    }
}

void QFileReader::setSource(QString source){
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

void QFileReader::setMonitor(bool monitor){
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

void QFileReader::systemFileChanged(const QString &){
    resync();
}

void QFileReader::resync(){
    if ( m_componentComplete && m_source != "" ){
        QFile fileInput(m_source);
        if ( !fileInput.open(QIODevice::ReadOnly ) ){
            lcv::Exception e = CREATE_EXCEPTION(lcv::Exception, "Cannot open file: " + m_source, 0);
            lcv::PluginContext::engine()->throwError(&e);
            return;
        }
        m_data = fileInput.readAll();
        emit dataChanged(m_data);
    }
}
