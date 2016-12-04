#include "qproject.h"
#include "qprojectfile.h"
#include "qprojectfilemodel.h"
#include "qprojectdocument.h"
#include <QUrl>

#include <QDebug>

namespace lcv{

QProject::QProject(QObject *parent)
    : QObject(parent)
    , m_fileModel(new QProjectFileModel(this))
    , m_focus(0)
    , m_active(0)
{
    newProject();
}

QProject::~QProject(){
}

void QProject::newProject(){
    m_fileModel->createProject();
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile()){
        QProjectDocument* document = new QProjectDocument(qobject_cast<QProjectFile*>(m_fileModel->root()->child(0)));
        m_openedFiles[""] = document;
        m_active = document;
        m_focus  = document;
        m_path   = "";
        emit pathChanged("");
        emit activeChanged(m_active);
        emit inFocusChanged(document);
    }
}

void QProject::openProject(const QString &path){
    closeProject();
    m_fileModel->openProject(path);
    emit pathChanged(path);

    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile()){
        QProjectDocument* document = new QProjectDocument(qobject_cast<QProjectFile*>(m_fileModel->root()->child(0)));
        m_openedFiles[document->file()->path()] = document;
        m_active = document;
        m_focus  = document;
        m_path   = path;
        emit inFocusChanged(document);
        emit activeChanged(document);
    }
//    emit pathChanged(path);
    //TODO: Look for active and in focus node
}

void QProject::closeProject(){
    m_focus = 0;
    m_active = 0;
    emit inFocusChanged(0);
    emit activeChanged(0);
    for( QHash<QString, QProjectDocument*>::iterator it = m_openedFiles.begin(); it != m_openedFiles.end(); ++it ){
        delete it.value();
    }
    m_openedFiles.clear();
    m_path = "";
    emit pathChanged("");
}

void QProject::openFile(const QString &path){
    QProjectDocument* document = 0;
    document = isOpened(path);
    if (!document){
        document = new QProjectDocument(m_fileModel->openFile(path));
        m_openedFiles[path] = document;
    }
    if (document)
        setInFocus(document);
}

void QProject::openFile(QProjectFile *file){
    if (!file)
        return;

    QProjectDocument* document = isOpened(file->path());
    if (!document){
        document = new QProjectDocument(file);
        file->setIsOpen(true);
        m_openedFiles[file->path()] = document;
    }
    if(document)
        setInFocus(document);
}

void QProject::setActive(QProjectFile* file){
    if (!file)
        return;

    QProjectDocument* document = isOpened(file->path());
    if (!document){
        document = new QProjectDocument(file);
        file->setIsOpen(true);
        m_openedFiles[file->path()] = document;
    }
    setActive(document);
}

void QProject::setActive(const QString &path){
    QProjectDocument* document = isOpened(path);
    if ( !document ){
        document = new QProjectDocument(m_fileModel->openFile(path));
    }
    if ( document ){
        setActive(document);
    }
}

QProjectDocument *QProject::isOpened(const QString &path){
    if ( m_openedFiles.contains(path) )
        return m_openedFiles[path];
    return 0;
}

void QProject::closeFocusedFile(){
    //TODO
}

void QProject::openFile(const QUrl &path){
    openFile(path.toLocalFile());
}

void QProject::setInFocus(QProjectDocument *document){
    if ( m_focus != document ){
        m_focus = document;
        emit inFocusChanged(document);
    }
}

void QProject::setActive(QProjectDocument *document){
    if ( m_active != document ){
        m_active = document;
        emit activeChanged(document);
    }
}

}// namespace
