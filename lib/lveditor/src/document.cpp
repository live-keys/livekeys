#include "document.h"
#include "live/project.h"
#include "live/visuallog.h"
#include "live/projectfile.h"
#include "live/lockedfileiosession.h"

#include <QFileInfo>

namespace lv{

Document::Document(ProjectFile *file, bool isMonitored, Project *parent)
    : QObject(parent)
    , m_file(file)
    , m_isMonitored(isMonitored)
    , m_isDirty(false)
{
    readContent();
    m_file->setDocument(this);
}


Document::Document(ProjectFile *file, Project *parent)
    : QObject(parent)
    , m_file(file)
    , m_isMonitored(false)
    , m_isDirty(false)
{
    m_file->setDocument(this);
}

Document::~Document(){
    if ( m_file->parent() == nullptr )
        m_file->deleteLater();
    else {
        m_file->setDocument(nullptr);
    }
}

/**
 * \brief Open the document in a read states
 */
void Document::readContent(){
    if ( m_file->exists() ){
        m_content = QByteArray::fromStdString(parentAsProject()->lockedFileIO()->readFromFile(m_file->path().toStdString()));
        m_lastModified = QFileInfo(m_file->path()).lastModified();
        emit contentChanged();
    }
}

/**
 * \brief Save modified document to its respective file
 */
bool Document::save(){
    if ( file()->exists() ){
        QByteArray data = content();
        if ( parentAsProject()->lockedFileIO()->writeToFile(
                 file()->path().toStdString(), data.constData(), static_cast<size_t>(data.length() )) )
        {
            setIsDirty(false);
            setLastModified(QDateTime::currentDateTime());
            emit saved();
            if ( parentAsProject() )
                parentAsProject()->documentSaved(this);
            return true;
        }
    }
    return false;
}


/**
 * \brief Save document content in a different file
 */
bool Document::saveAs(const QString& path){
    if ( m_file->path() == path ){
        save();
    } else if ( path != "" ){
        QByteArray data = content();
        if ( parentAsProject()->lockedFileIO()->writeToFile(
                 path.toStdString(), data, static_cast<size_t>(data.size())) ){
            ProjectFile* file = parentAsProject()->relocateDocument(m_file->path(), path, this);
            if ( file ){
                m_file->setDocument(nullptr);
                m_file = file;
                emit fileChanged();
            }
            setIsDirty(false);
            emit saved();
            m_lastModified = QDateTime::currentDateTime();
            parentAsProject()->documentSaved(this);
            return true;
        }
    }
    return false;
}

/**
 * \brief Variant of saveAs using QUrl parameter
 *
 * \sa ProjectDocument::saveAs(const QString &path)
 */
bool Document::saveAs(const QUrl& url){
    return saveAs(url.toLocalFile());
}


/**
 * \brief Returns the parent, which is a Project
 */
Project *Document::parentAsProject(){
    return qobject_cast<Project*>(parent());
}

}// namespace
