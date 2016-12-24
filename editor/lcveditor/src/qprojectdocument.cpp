#include "qprojectdocument.h"
#include "qprojectfile.h"
#include "qlockedfileiosession.h"
#include "qproject.h"

#include <QFile>
#include <QUrl>
#include <QTextStream>
#include <QFileInfo>

namespace lcv{

QProjectDocument::QProjectDocument(QProjectFile *file, bool isMonitored, QProject *parent)
    : QObject(parent)
    , m_file(file)
    , m_isMonitored(isMonitored)
{
    readContent();
}

void QProjectDocument::dumpContent(const QString &content){
    m_content = content;
    emit contentChanged();
}

void QProjectDocument::readContent(){
    if ( m_file->path() != "" ){
        m_content = parentAsProject()->lockedFileIO()->readFromFile(m_file->path());
        m_lastModified = QFileInfo(m_file->path()).lastModified();
        emit contentChanged();
    }
}

QProject *QProjectDocument::parentAsProject(){
    return qobject_cast<QProject*>(parent());
}

bool QProjectDocument::save(){
    if ( m_file->path() != "" ){
        if ( parentAsProject()->lockedFileIO()->writeToFile(m_file->path(), m_content ) ){
            m_file->setIsDirty(false);
            m_lastModified = QDateTime::currentDateTime();
            return true;
        }
    }
    return false;
}

bool QProjectDocument::saveAs(const QString &path){
    if ( path != "" ){
        if ( parentAsProject()->lockedFileIO()->writeToFile(path, m_content ) ){
            m_file->setIsDirty(false);
            m_lastModified = QDateTime::currentDateTime();
            return true;
        }
    }
    return false;
}

bool QProjectDocument::saveAs(const QUrl &url){
    return saveAs(url.toLocalFile());
}

QProjectDocument::~QProjectDocument(){
    if ( m_file->parent() == 0 )
        m_file->deleteLater();
    else {
        m_file->setIsOpen(false);
        m_file->setIsDirty(false);
    }
}

}// namespace
