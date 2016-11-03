#include "qproject.h"
#include "qprojectfile.h"
#include "qprojectfilemodel.h"

namespace lcv{

QProject::QProject(QObject *parent)
    : QObject(parent)
    , m_fileModel(new QProjectFileModel(this))
{
}

QProject::~QProject(){
}

void QProject::openProject(const QString &path){

}

void QProject::closeProject(){

}

void QProject::openFile(const QString &path){
    QProjectFile* file = 0;
    file = isOpened(path);
    if ( !file )
        file = m_fileModel->openFile(path);
    if ( file )
        m_focus = file;
}

void QProject::setActive(const QString &path){
    QProjectFile* file = 0;
    file = isOpened(path);
    if ( !file )
        file = m_fileModel->openFile(path);
    if ( file ){
        if ( m_active != file ){
            m_active = file;
            emit activeChanged(file);
        }
    }
}

QProjectFile *QProject::isOpened(const QString &path){
    if ( m_openedFiles.contains(path) )
        return m_openedFiles[path];
    return 0;
}

void QProject::closeFocusedFile(){

}

}// namespace
