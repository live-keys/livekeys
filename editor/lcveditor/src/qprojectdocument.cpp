#include "qprojectdocument.h"
#include "qprojectfile.h"

#include <QFile>
#include <QTextStream>

namespace lcv{

QProjectDocument::QProjectDocument(QProjectFile *file, QObject *parent)
    : QObject(parent)
    , m_file(file)
{
    readContent();
}

void QProjectDocument::dumpContent(const QString &content){
    m_content = content;
    emit contentChanged();
}

void QProjectDocument::readContent(){
    if ( m_file->path() != "" ){
        QFile fileInput(m_file->path());
        if ( !fileInput.open(QIODevice::ReadOnly ) )
            qCritical("Cannot open file: %s", qPrintable(m_file->path()));

        QTextStream in(&fileInput);
        m_content = in.readAll();
        fileInput.close();
    }
}

void QProjectDocument::save(){
    if ( m_file->path() != "" ){
        QFile fileInput(m_file->path());
        if ( !fileInput.open(QIODevice::WriteOnly ) ){
            qCritical("Can't open file for writing");
            return;
        } else {
            QTextStream stream(&fileInput);
            stream << m_content;
            stream.flush();
            fileInput.close();
            m_file->setIsDirty(false);
        }
    }
}

QProjectDocument::~QProjectDocument(){
}

}// namespace
