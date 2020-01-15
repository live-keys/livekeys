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

#include "live/project.h"
#include "live/projectfile.h"
#include "live/projectentry.h"
#include "live/projectdocument.h"
#include "live/projectdocumentmodel.h"
#include "live/viewengine.h"
#include "projectnavigationmodel.h"
#include "projectfilemodel.h"
#include "runnablecontainer.h"
#include "runnable.h"

#include <QFileInfo>
#include <QUrl>
#include <QQuickItem>
#include <QDebug>
#include <QTimer>
#include <QCryptographicHash>

/**
 * \class lv::Project
 * \brief Abstraction of the open project in LiveKeys
 *
 * Practically a singleton (not enforced), constructed at the start of the application, and destroyed right before closing.
 * There are two types of projects - file-based projects and folder-based projects.
 * \ingroup lveditor
 */
namespace lv{

/**
 * \brief Default constructor
 */
Project::Project(el::Engine *engine, QObject *parent)
    : QObject(parent)
    , m_fileModel(new ProjectFileModel(this))
    , m_navigationModel(new ProjectNavigationModel(this))
    , m_documentModel(new ProjectDocumentModel(this))
    , m_runnables(new RunnableContainer(this))
    , m_lockedFileIO(LockedFileIOSession::createInstance())
    , m_active(nullptr)
    , m_runspace(nullptr)
    , m_scheduleRunTimer(new QTimer())
    , m_runTrigger(Project::RunOnChange)
    , m_engine(engine)
{
    m_scheduleRunTimer->setInterval(1000);
    m_scheduleRunTimer->setSingleShot(true);
    connect(m_scheduleRunTimer, &QTimer::timeout, this, &Project::run);

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &Project::closeProject);
}

/**
 * \brief Default destructor
 */
Project::~Project(){
    delete m_documentModel;
    delete m_fileModel;
    delete m_navigationModel;
    delete m_scheduleRunTimer;
}

/**
 * \brief Creates a simple demo file project, and closes the previously opened one (if it exists)
 *
 * Nameless file with a simple Grid
 */
void Project::newProject(){
    m_active = nullptr;
    m_runnables->clearAll();
    m_fileModel->createProject();
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile()){
        ProjectDocument* document = createTextDocument(
            qobject_cast<ProjectFile*>(m_fileModel->root()->child(0)), false
        );
	        
        document->addEditingState(ProjectDocument::Read);
        document->setContent("import QtQuick 2.3\n\nGrid{\n}");
        document->removeEditingState(ProjectDocument::Read);
        m_documentModel->openDocument("T:0", document);

        m_active = new Runnable(viewEngine(), m_engine, "T:0", m_runnables, "untitled");
        m_active->setRunSpace(m_runspace);
        m_runnables->addRunnable(m_active);
        setActive(m_active);

        m_path   = "";
        emit pathChanged("");
        emit activeChanged(m_active);

        scheduleRun();
    }
}

/**
 * \brief Open project given by the path
 *
 * It can be either a single file or an entire folder, as mentioned before
 */
void Project::openProject(const QString &path){
    QFileInfo pathInfo(path);
    if ( !pathInfo.exists() ){
        qCritical("Path does not exist: %s", qPrintable(path));
        return;
    }
    closeProject();
    QString absolutePath = QFileInfo(path).absoluteFilePath();
    m_fileModel->openProject(absolutePath);

    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile()){
        ProjectDocument* document = createTextDocument(
            qobject_cast<ProjectFile*>(m_fileModel->root()->child(0)),
            false
        );

        m_documentModel->openDocument(document->file()->path(), document);

        Runnable* r = new Runnable(viewEngine(), m_engine, document->file()->path(), m_runnables, document->file()->name());
        r->setRunSpace(m_runspace);
        m_runnables->addRunnable(r);
        m_active = r;

        m_path   = absolutePath;
        emit pathChanged(absolutePath);
        emit activeChanged(m_active);
    } else if ( m_fileModel->root()->childCount() > 0 ){
        m_path = absolutePath;
        emit pathChanged(absolutePath);

        if ( !m_active ){
            ProjectFile* bestFocus = lookupBestFocus(m_fileModel->root()->child(0));
            if( bestFocus ){
                setActive(bestFocus->path());
            }
        }
    }

    scheduleRun();
}

/** Opens project given by QUrl */
void Project::openProject(const QUrl &url){
    openProject(url.toLocalFile());
}

/**
 * \brief Returns the path of the folder of the given file-based project, or the actual folder of a folder-based project
 */
QString Project::dir() const{
    if ( m_path.isEmpty() )
        return m_path;
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile())
        return QFileInfo(m_path).path();
    else
        return m_path;
}

ProjectFile *Project::relocateDocument(const QString &path, const QString& newPath, Document *document){
    m_documentModel->relocateDocument(path, newPath, document);
    QString absoluteNewPath = QFileInfo(newPath).absoluteFilePath();
    if (absoluteNewPath.indexOf(m_path) == 0 )
        m_fileModel->rescanEntries();
    return m_fileModel->openFile(newPath);
}

/**
 * \brief Closes the currently opened project
 *
 * It doesn't destroy the singleton capital-P Project, simply leaves it in a blank state
 */
void Project::closeProject(){
    emit aboutToClose();
    Runnable* r = nullptr;
    setActive(r);
    m_documentModel->closeDocuments();
    m_fileModel->closeProject();
    m_runnables->clearAll();
    m_path = "";
    emit pathChanged("");
}

/**
 * \brief QUrl variant of the other openFile function
 *
 * \sa Project::openFile(const QString &path, int mode)
 */
ProjectDocument *Project::openTextFile(const QUrl &path, int mode){
    return openTextFile(path.toLocalFile(), mode);
}

/**
 * \brief Opens the file given by the \p path, in the given mode
 *
 * If the document is not opened, we use the third openFile function to do so.
 * If it is, we update its monitoring state.
 *
 * \sa Project::openFile(ProjectFile *file, int mode)
 */
ProjectDocument *Project::openTextFile(const QString &path, int mode){
    Document* document = isOpened(path);

    ProjectDocument* projectDocument = ProjectDocument::castFrom(document);
    if ( document && !projectDocument ){
        closeFile(document->file()->path());
    }

    if (!projectDocument){
        return openTextFile(m_fileModel->openFile(path), mode);
    } else if ( projectDocument->isMonitored() && mode == ProjectDocument::Edit ){
        m_documentModel->updateDocumentMonitoring(projectDocument, false);
    } else if ( !projectDocument->isMonitored() && mode == ProjectDocument::Monitor ){
        projectDocument->readContent();
        m_documentModel->updateDocumentMonitoring(projectDocument, true);
    }

    return projectDocument;
}

/**
 * \brief Opens the given file in the given mode, this time using the internal ProjectFile object
 *
 * All open documents are immediately added to the document model
 */
ProjectDocument *Project::openTextFile(ProjectFile *file, int mode){
    if (!file)
        return nullptr;

    Document* document = isOpened(file->path());

    ProjectDocument* projectDocument = ProjectDocument::castFrom(document);
    if ( document && !projectDocument ){
        closeFile(document->file()->path());
    }

    if (!projectDocument){
        projectDocument = createTextDocument(file, (mode == ProjectDocument::Monitor));
        m_documentModel->openDocument(file->path(), projectDocument);
    } else if ( projectDocument->isMonitored() && mode == ProjectDocument::Edit ){
        m_documentModel->updateDocumentMonitoring(projectDocument, false);
    } else if ( !projectDocument->isMonitored() && mode == ProjectDocument::Monitor ){
        projectDocument->readContent();
        m_documentModel->updateDocumentMonitoring(projectDocument, true);
    }

    return projectDocument;
}

Document *Project::openFile(const QString &path, int mode){
    Document* document = isOpened(path);

    if ( ProjectDocument::castFrom(document) ){
        closeFile(document->file()->path());
        document = nullptr;
    }

    if (!document){
        return openFile(m_fileModel->openFile(path), mode);
    } else if ( document->isMonitored() && mode == ProjectDocument::Edit ){
        m_documentModel->updateDocumentMonitoring(document, false);
    } else if ( !document->isMonitored() && mode == ProjectDocument::Monitor ){
        document->readContent();
        m_documentModel->updateDocumentMonitoring(document, true);
    }

    return document;
}

Document *Project::openFile(ProjectFile *file, int mode){
    if (!file)
        return nullptr;

    Document* document = isOpened(file->path());
    if ( ProjectDocument::castFrom(document) ){
        closeFile(document->file()->path());
        document = nullptr;
    }

    if (!document){
        document = createDocument(file, (mode == ProjectDocument::Monitor));
        m_documentModel->openDocument(file->path(), document);
    } else if ( document->isMonitored() && mode == ProjectDocument::Edit ){
        m_documentModel->updateDocumentMonitoring(document, false);
    } else if ( !document->isMonitored() && mode == ProjectDocument::Monitor ){
        document->readContent();
        m_documentModel->updateDocumentMonitoring(document, true);
    }

    return document;
}

/**
 * \brief Shows if the project is of folder type
 */
bool Project::isDirProject() const{
    return m_fileModel->root()->childCount() > 0 && !m_fileModel->root()->child(0)->isFile();
}

/**
 * \brief QUrl variant of this function
 *
 * \sa Project::isFileInProject(const QString &path)
 */
bool Project::isFileInProject(const QUrl &path) const{
    return isFileInProject(path.toLocalFile());
}

/**
 * \brief Shows if the file given by the QString path is inside this folder-based project
 */
bool Project::isFileInProject(const QString &path) const{
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile() )
        return path == m_path;
    else
        return !path.isEmpty() && path.startsWith(m_path);
}

/**
 * \brief Set the active file given its path
 *
 * This file is the one actually compiling
 */
void Project::setActive(const QString &path){
    Runnable* r = openRunnable(path);
    if (r){
        r->setRunSpace(m_runspace);
        setActive(r);
    }
}

Runnable *Project::openRunnable(const QString &path, const QStringList &activations){
    Runnable* r = m_runnables->runnableAt(path);
    if ( !r ){
        ProjectDocument* document = ProjectDocument::castFrom(isOpened(path));

        QSet<QString> activ;
        for ( auto it = activations.begin(); it != activations.end(); ++it )
            activ.insert(*it);

        if ( document ){
            r = new Runnable(viewEngine(), m_engine, document->file()->path(), m_runnables, document->file()->name(), activ);
        } else {
            QFileInfo pathInfo(path);
            r = new Runnable(viewEngine(), m_engine, path, m_runnables, pathInfo.fileName(), activ);
        }

        m_runnables->addRunnable(r);
    }


    return r;
}

/**
 * \brief Finds the "best" file in the project to be the inital active file
 *
 * The preference is for "main.qml" first, then the first lowercase-named file
 */
ProjectFile *Project::lookupBestFocus(ProjectEntry *entry){
    QList<ProjectEntry*> entriesToScan;
    if ( entry->lastCheckTime().isNull() )
        m_fileModel->expandEntry(entry);

    ProjectFile* bestEntry = nullptr;

    foreach( ProjectEntry* childEntry, entry->entries() ){
        if ( childEntry->isFile() ){
            if ( !childEntry->name().isEmpty() ){
                if ( childEntry->name() == "main.qml" ){
                    return qobject_cast<ProjectFile*>(childEntry);
                } else if ( childEntry->name().endsWith(".qml") ) {
                    if ( !bestEntry ) {
                        bestEntry = qobject_cast<ProjectFile*>(childEntry);
                    } else if ( childEntry->name().at(0).isLower() && !bestEntry->name().at(0).isLower() ){
                        bestEntry = qobject_cast<ProjectFile*>(childEntry);
                    }
                }
            }
        } else {
            entriesToScan.append(childEntry);
        }
    }

    foreach( ProjectEntry* entry, entriesToScan ){
        ProjectFile* file = lookupBestFocus(entry);
        if ( file ){
            if ( file->name() == "main.qml" ){
                return qobject_cast<ProjectFile*>(file);
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

/**
 * \brief Checks whether there's a file opened at this specified path
 */
Document *Project::isOpened(const QString &path){
    return m_documentModel->isOpened(path);
}

/**
 * \brief Sets the load position for the project.
 */
void Project::setRunSpace(QObject *runspace){
    m_runspace = runspace;
    if ( m_active )
        m_active->setRunSpace(runspace);
}

/**
 * Generates a hash out of a given path
 */
QByteArray Project::hashPath(const QByteArray &path){
    return QCryptographicHash::hash(path, QCryptographicHash::Md5);
}

/**
 * \brief Exclude these paths from scheduling a run
 */
void Project::excludeRunTriggers(const QSet<QString> &paths){
    for ( auto it = paths.begin(); it != paths.end(); ++it ){
        m_excludedRunTriggers.insert(*it);
    }
}

/**
 * \brief Remove these paths from excluding a scheduled run
 */
void Project::removeExcludedRunTriggers(const QSet<QString> &paths){
    for ( auto it = paths.begin(); it != paths.end(); ++it ){
        m_excludedRunTriggers.remove(*it);
    }
}

/**
 * \brief Closes the file given the path
 */
void Project::closeFile(const QString &path){
    m_documentModel->closeDocument(path);
}

void Project::setActive(Runnable* runnable){
    if ( m_active != runnable ){
        m_active = runnable;
        emit activeChanged(runnable);

        scheduleRun();
    }
}

ProjectDocument *Project::createTextDocument(ProjectFile *file, bool isMonitored){
    ProjectDocument* document = new ProjectDocument(file, isMonitored, this);

    connect(document->textDocument(), &QTextDocument::contentsChange, [this, document](int, int, int){

        if ( m_excludedRunTriggers.contains(document->file()->path()) )
            return;

        if ( document->editingStateIs(ProjectDocument::Read) && document->isMonitored() ){
            if ( m_runTrigger != Project::RunManual )
                scheduleRun();
            return;
        }

        if ( document->editingStateIs(ProjectDocument::Read) ||
             document->editingStateIs(ProjectDocument::Overlay) ||
             document->editingStateIs(ProjectDocument::Silent ) )
        {
            return;
        }
        if ( m_runTrigger == Project::RunOnChange )
            scheduleRun();
    });

    emit documentOpened(document);

    return document;
}

Document *Project::createDocument(ProjectFile *file, bool isMonitored){
    Document* document = new Document(file, isMonitored, this);
    emit documentOpened(document);
    return document;
}

void Project::documentSaved(Document *document){
    emit fileChanged(document->file()->path());
}

/**
 * \brief Returns an absolute path given a relative one
 */
QString Project::path(const QString &relative) const{
    return dir() + '/' + relative;
}

/**
 * \brief Schedules a run in 1 second. Clears any previous schedule.
 */
void Project::scheduleRun(){
    m_scheduleRunTimer->start();
}

/**
 * \brief Run the current project
 */
void Project::run(){
    if ( !m_active )
        return;

    m_active->run();
}

QObject *Project::runSpace(){
    return m_active ? m_active->runSpace() : nullptr;
}

QObject *Project::appRoot(){
    return m_active ? m_active->viewRoot() : nullptr;
}

ViewEngine *Project::viewEngine(){
    return qobject_cast<ViewEngine*>(parent());
}

}// namespace
