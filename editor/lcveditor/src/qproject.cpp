#include "qproject.h"
#include "qprojectfile.h"
#include "qprojectentry.h"
#include "qprojectfilemodel.h"
#include "qprojectdocument.h"
#include "qprojectnavigationmodel.h"
#include "qprojectdocumentmodel.h"

#include <QFileInfo>
#include <QUrl>
#include <QDebug>

namespace lcv{

QProject::QProject(QObject *parent)
    : QObject(parent)
    , m_fileModel(new QProjectFileModel(this))
    , m_navigationModel(new QProjectNavigationModel(this))
    , m_documentModel(new QProjectDocumentModel(this))
    , m_focus(0)
    , m_active(0)
{
    newProject();
}

QProject::~QProject(){
    delete m_fileModel;
    delete m_navigationModel;
    delete m_documentModel;
}

void QProject::newProject(){
    m_fileModel->createProject();
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile()){
        QProjectDocument* document = new QProjectDocument(qobject_cast<QProjectFile*>(m_fileModel->root()->child(0)));
        m_documentModel->openDocument("", document);
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
    QString absolutePath = QFileInfo(path).absoluteFilePath();
    m_fileModel->openProject(absolutePath);

    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile()){
        QProjectDocument* document = new QProjectDocument(qobject_cast<QProjectFile*>(m_fileModel->root()->child(0)));
        m_documentModel->openDocument(document->file()->path(), document);
        m_active = document;
        m_focus  = document;
        m_path   = absolutePath;
        emit pathChanged(absolutePath);
        emit inFocusChanged(document);
        emit activeChanged(document);
    } else if ( m_fileModel->root()->childCount() > 0 ){

        QProjectFile* bestFocus = lookupBestFocus(m_fileModel->root()->child(0));
        if( bestFocus ){
            QProjectDocument* document = new QProjectDocument(bestFocus);
            m_documentModel->openDocument(document->file()->path(), document);
            m_active = document;
            m_focus = document;
            emit inFocusChanged(document);
            emit activeChanged(document);
        }

        m_path = absolutePath;
        emit pathChanged(absolutePath);
    }
}

void QProject::closeProject(){
    m_focus = 0;
    m_active = 0;
    emit inFocusChanged(0);
    emit activeChanged(0);
    m_documentModel->closeDocuments();
    m_path = "";
    emit pathChanged("");
}

void QProject::openFile(const QString &path){
    QProjectDocument* document = isOpened(path);
    if (!document)
        openFile(m_fileModel->openFile(path));
    else
        setInFocus(document);
}

void QProject::openFile(QProjectFile *file){
    if (!file)
        return;

    QProjectDocument* document = isOpened(file->path());
    if (!document){
        document = new QProjectDocument(file);
        file->setIsOpen(true);
        m_documentModel->openDocument(file->path(), document);
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
        m_documentModel->openDocument(file->path(), document);
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

QProjectFile *QProject::lookupBestFocus(QProjectEntry *entry){
    QList<QProjectEntry*> entriesToScan;
    if ( entry->lastCheckTime().isNull() )
        m_fileModel->expandEntry(entry);

    QProjectFile* bestEntry = 0;

    foreach( QProjectEntry* childEntry, entry->entries() ){
        if ( childEntry->isFile() ){
            if ( !childEntry->name().isEmpty() ){
                if ( childEntry->name() == "main.qml" ){
                    return qobject_cast<QProjectFile*>(childEntry);
                } else if ( childEntry->name().endsWith(".qml") ) {
                    if ( !bestEntry ) {
                        bestEntry = qobject_cast<QProjectFile*>(childEntry);
                    } else if ( childEntry->name().at(0).isLower() && !bestEntry->name().at(0).isLower() ){
                        bestEntry = qobject_cast<QProjectFile*>(childEntry);
                    }
                }
            }
        } else {
            entriesToScan.append(childEntry);
        }
    }

    foreach( QProjectEntry* entry, entriesToScan ){
        QProjectFile* file = lookupBestFocus(entry);
        if ( file ){
            if ( file->name() == "main.qml" ){
                return qobject_cast<QProjectFile*>(file);
            } else if ( file->name().endsWith(".qml") ) {
                if ( !bestEntry ) {
                    bestEntry = file;
                } else if ( file->name().at(0).isLower() && !bestEntry->name().at(0).isLower() ){
                    bestEntry = file;
                }
            }
        }
    }

    return bestEntry;
}

QProjectDocument *QProject::isOpened(const QString &path){
    return m_documentModel->isOpened(path);
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
