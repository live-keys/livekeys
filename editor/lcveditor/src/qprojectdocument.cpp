#include "qprojectdocument.h"
#include "qprojectfile.h"
#include "qlockedfileio.h"

#include <QFile>
#include <QTextStream>

namespace lcv{

QProjectDocument::QProjectDocument(QProjectFile *file, QObject *parent)
    : QObject(parent)
    , m_file(file)
{
    //HERE: Get last modified datetime
    readContent();
}

void QProjectDocument::dumpContent(const QString &content){
    m_content = content;
    emit contentChanged();
}

void QProjectDocument::readContent(){
    if ( m_file->path() != "" ){
        m_content = QLockedFileIO::instance().readFromFile(m_file->path());
    }
}

void QProjectDocument::save(){
    if ( m_file->path() != "" ){
        if ( QLockedFileIO::instance().writeToFile(m_file->path(), m_content ) ){
            m_file->setIsDirty(false);
        }
    }
}

QProjectDocument::~QProjectDocument(){
    if ( m_file->parent() == 0 )
        m_file->deleteLater();
}

}// namespace
