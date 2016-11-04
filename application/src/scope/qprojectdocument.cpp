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

QProjectDocument::~QProjectDocument(){
}

}// namespace
