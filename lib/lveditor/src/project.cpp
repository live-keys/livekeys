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
#include "live/exception.h"
#include "live/visuallogqt.h"
#include "live/programholder.h"
#include "live/program.h"
#include "live/qmlprogram.h"
#include "workspacelayer.h"
#include "fileformattypes.h"
#include "projectfileindexer.h"
#include "projectfilemodel.h"
#include "projectengineinterceptor.h"
#include "runnablecontainer.h"
#include "runnable.h"

#include <QFileInfo>
#include <QDir>
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
Project::Project(WorkspaceLayer *workspaceLayer, QObject *parent)
    : QObject(parent)
    , m_workspaceLayer(workspaceLayer)
    , m_fileModel(new ProjectFileModel(this))
    , m_documentModel(new ProjectDocumentModel(this))
    , m_runnables(new RunnableContainer(this))
    , m_programHolder(nullptr)
    , m_active(nullptr)
    , m_runspace(nullptr)
    , m_scheduleRunTimer(new QTimer())
    , m_runTrigger(Project::RunOnChange)
    , m_engine(nullptr)
{
    ViewEngine* ve = viewEngine();
    QObject* lkobject = ve->engine()->rootContext()->contextProperty("lk").value<QObject*>();
    m_programHolder = dynamic_cast<ProgramHolder*>(lkobject);

    ProjectEngineInterceptor* interceptor = new ProjectEngineInterceptor(ve, this);
    ve->setInterceptor(interceptor);

    m_scheduleRunTimer->setInterval(1000);
    m_scheduleRunTimer->setSingleShot(true);
    connect(m_scheduleRunTimer, &QTimer::timeout, this, &Project::run);
    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, &Project::closeProject);

    if ( m_programHolder && m_programHolder->main() ){
        QmlProgram* qmlProgram = static_cast<QmlProgram*>(m_programHolder->main());
        if ( qmlProgram ){
            openProject(QUrl::fromLocalFile(QString::fromStdString(qmlProgram->rootPath().data())), qmlProgram);
        }
    }
}

/**
 * \brief Default destructor
 */
Project::~Project(){
    delete m_documentModel;
    delete m_fileModel;
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
            qobject_cast<ProjectFile*>(m_fileModel->root()->child(0)), "qml", false
        );

        document->addEditingState(ProjectDocument::Read);
        document->setContent("import QtQuick 2.3\n\nGrid{\n}");
        document->removeEditingState(ProjectDocument::Read);
        m_documentModel->openDocument("T:0", document);

        QmlProgram* qmlProgram = QmlProgram::create(viewEngine(), "", "T:0");
        m_active = new Runnable(qmlProgram, m_runnables);
        m_active->setRunSpace(m_runspace);
        m_runnables->addRunnable(m_active);

        m_programHolder->setMain(qmlProgram);

        setActive(m_active);

        m_path = "";
        emit pathChanged("");
        emit activeChanged(m_active);

        scheduleRun();
    }
}


void Project::openProject(const QUrl &url, Program *main){
    if ( !url.isLocalFile() )
        THROW_EXCEPTION(Exception, Utf8("Failed to open url: %").format(url), Exception::toCode("~Url"));
    QString path = localPath(url);
    QFileInfo pathInfo(path);
    if ( !pathInfo.exists() ){
        THROW_EXCEPTION(Exception, Utf8("Path does not exist: %").format(path), Exception::toCode("~Path"));
    }
    closeProject();
    QString absolutePath = QFileInfo(path).absoluteFilePath();

    m_fileModel->openProject(absolutePath);

    QmlProgram* qmlProgram = static_cast<QmlProgram*>(main);
    if ( qmlProgram && !qmlProgram->mainPath().isEmpty() ){
        Runnable* r = new Runnable(qmlProgram, m_runnables);
        r->setRunSpace(m_runspace);
        m_runnables->addRunnable(r);
        m_active = r;
    }

    m_path   = absolutePath;
    emit pathChanged(absolutePath);
    emit activeChanged(m_active);
}

QString Project::findFileFormat(const QString &path){
    return m_workspaceLayer->fileFormats()->find(path);
}

QString Project::localPath(const QUrl &url){
    QString result = url.toLocalFile();
    if ( result.startsWith("/T:") )
        return result.mid(1);
    return result;
}

/**
 * \brief Open project given by the path
 *
 * It can be either a single file or an entire folder, as mentioned before
 */
void Project::openProject(const QUrl &url){
    QmlProgram* qmlMain = nullptr;
    try{
        qmlMain = QmlProgram::create(viewEngine(), url.toLocalFile().toStdString());
        openProject(url, qmlMain);
        m_programHolder->setMain(qmlMain);
    } catch ( lv::Exception& e ){
        delete qmlMain;
        QmlError(viewEngine(), e, this).jsThrow();
        return;
    }
}

Document *Project::isOpened(const QUrl &url){
    if ( !url.isLocalFile() ){
        Exception e = CREATE_EXCEPTION(
            Exception, Utf8("Failed to open url: %").format(url), Exception::toCode("~Url"));
        QmlError(viewEngine(), e, this).jsThrow();
        return nullptr;
    }
    return isOpened(localPath(url));
}

/**
 * \brief Opens a file using the given options
 * \p Options can be { type: 'binary', mode: editIfNotOpen, format: 'jpg' }
 */
Document* Project::openFile(const QUrl &url, const QJSValue &opt){
    if ( !url.isLocalFile() ){
        Exception e = CREATE_EXCEPTION(Exception, "Failed to open url: " + url.toString().toStdString(), Exception::toCode("~Url"));
        QmlError(viewEngine(), e, this).jsThrow();
        return nullptr;
    }
    if ( !opt.isObject() && !opt.isNull() ){
        Exception e = CREATE_EXCEPTION(Exception, "Invalid opt argument provided: " + opt.toString().toStdString(), Exception::toCode("~Url"));
        QmlError(viewEngine(), e, this).jsThrow();
        return nullptr;
    }

    QString path = localPath(url);
    QString type = opt.hasOwnProperty("type") ? opt.property("type").toString() : "binary";
    int mode = opt.hasOwnProperty("mode") ? opt.property("mode").toInt() : Document::EditIfNotOpen;
    QString format = opt.hasOwnProperty("format")
            ? opt.property("format").toString()
            : findFileFormat(path);

    try {
        return openFile(path, type, mode, format);
    }  catch (lv::Exception& e) {
        QmlError(viewEngine(), e, this).jsThrow();
    }

    return nullptr;
}

Document *Project::createDocument(const QJSValue &opt){
    bool fileSystem = opt.hasOwnProperty("fileSystem") ? opt.property("fileSystem").toBool() : false;
    QString type = opt.hasOwnProperty("type") ? opt.property("type").toString() : "binary";
    QString format = opt.hasOwnProperty("format") ? opt.property("format").toString() : "";

    if ( fileSystem ){
        ProjectFile* fe = m_fileModel->addTemporaryFile();
        return openFile(fe->path(), type, lv::Document::Edit, format);
    } else {
        if ( type == "binary" ){
            Document* document = new Document(nullptr, format, false, nullptr);
            QQmlEngine::setObjectOwnership(document, QQmlEngine::JavaScriptOwnership);
            return document;
        } else if ( type == "text" ){
            ProjectDocument* document = new ProjectDocument(nullptr, format, false, nullptr);
            QQmlEngine::setObjectOwnership(document, QQmlEngine::JavaScriptOwnership);
            return document;
        } else {
            Exception e = CREATE_EXCEPTION(Exception, Utf8("Failed to create document of type: %").format(type), Exception::toCode("~Type"));
            QmlError(viewEngine(), e, this).jsThrow();
            return nullptr;
        }
    }
    return nullptr;
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
    document->setPath(newPath);

    QString absoluteNewPath = QFileInfo(newPath).absoluteFilePath();
    if (absoluteNewPath.indexOf(m_path) == 0 )
        m_fileModel->rescanEntries();

    ProjectFile* file = m_fileModel->openFile(newPath);
    if ( file ){
        file->setDocument(document);
    }
    return file;
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
 * \brief Shows if the project is of folder type
 */
bool Project::isDirProject() const{
    return m_fileModel->root()->childCount() > 0 && !m_fileModel->root()->child(0)->isFile();
}

/**
 * \brief Shows if the file given by the \p path is inside this folder-based project
 */
bool Project::isFileInProject(const QUrl &url){
    if ( !url.isLocalFile() ){
        Exception e = CREATE_EXCEPTION(Exception, "Failed to open url: " + url.toString().toStdString(), Exception::toCode("~Url"));
        QmlError(viewEngine(), e, this).jsThrow();
        return false;
    }

    QString path = localPath(url);
    if ( m_fileModel->root()->childCount() > 0 && m_fileModel->root()->child(0)->isFile() )
        return path == m_path;
    else
        return !path.isEmpty() && path.startsWith(m_path);
}

/**
 * \brief Checks whether the file can be run
 */
bool Project::canRunFile(const QString &path) const{
    return path.endsWith(".qml") || path.endsWith(".lv");
}

/**
 * \brief Set the active file given its path
 *
 * This file is the one actually compiling
 */
void Project::setActive(const QString &path){
    Runnable* r = openRunnable(path);
    if (r){
        setActive(r);
    }
}

Runnable *Project::openRunnable(const QString &path, const QStringList &activations){
    Runnable* r = m_runnables->runnableAt(path);
    if (!r){
        QSet<QString> activ;
        for ( auto it = activations.begin(); it != activations.end(); ++it )
            activ.insert(*it);
        QmlProgram* qmlProgram = QmlProgram::create(viewEngine(), m_path.toStdString(), path.toStdString());
        if ( qmlProgram && !qmlProgram->mainPath().isEmpty() ){
            vlog() << qmlProgram->mainPath();
            r = new Runnable(qmlProgram, m_runnables, activ);
            m_runnables->addRunnable(r);
        }
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

void Project::monitorFiles(const QStringList &files){
    for( const QString& mfile: files ){
        if ( !mfile.isEmpty() ){
            QFileInfo mfileInfo(mfile);
            if ( mfileInfo.isRelative() ){
                QString filePath = QDir::cleanPath(rootPath() + QDir::separator() + mfile);
                openFile(
                    filePath,
                    "text",
                    ProjectDocument::Monitor,
                    findFileFormat(filePath)
                );
            } else {
                openFile(mfile, "text", ProjectDocument::Monitor, findFileFormat(mfile));
            }
        }
    }
}

/**
 * \brief Opens the file given by the \p path, in the given mode
 *
 * If the document is not opened, we use the third openFile function to do so.
 * If it is, we update its monitoring state.
 *
 * \sa Project::openFile(ProjectFile *file, int mode)
 */
Document* Project::openFile(const QString &path, const QString &type, int mode, const QString &format){
    Document* document = isOpened(path);
    if ( type == "binary" ){
        if ( document ){
            if ( ProjectDocument::castFrom(document) ){
                closeFile(document->path());
                document = nullptr;
            }
        }
        if ( !document ){
            ProjectFile* file = m_fileModel->openFile(path);
            if (!file)
                return nullptr;
            document = createDocument(file, format, (mode == ProjectDocument::Monitor));
            m_documentModel->openDocument(file->path(), document);
        }
    } else if ( type == "text" ){
        if ( document ){
            if ( !ProjectDocument::castFrom(document) ){
                closeFile(document->path());
                document = nullptr;
            }
        }
        if ( !document ){
            ProjectFile* file = m_fileModel->openFile(path);
            if (!file)
                return nullptr;
            document = createTextDocument(file, format, (mode == ProjectDocument::Monitor));
            m_documentModel->openDocument(file->path(), document);
        }
    } else {
        THROW_EXCEPTION(Exception, Utf8("Failed to create document of type: %").format(type), Exception::toCode("~Document"));
    }

    if ( !document ){
        THROW_EXCEPTION(Exception, Utf8("Failed to create document at: %").format(path), Exception::toCode("~Document"));
        return nullptr;
    }

    if ( document->isMonitored() && mode == ProjectDocument::Edit ){
        m_documentModel->updateDocumentMonitoring(document, false);
    } else if ( !document->isMonitored() && mode == ProjectDocument::Monitor ){
        document->readContent();
        m_documentModel->updateDocumentMonitoring(document, true);
    }

    return document;
}

/**
 * \brief Closes the file given the path
 */
void Project::closeFile(const QString &path){
    m_documentModel->closeDocument(path);
}

void Project::setActive(Runnable* runnable){
    if ( m_active != runnable ){
        if ( m_active ){
            m_active->setRunSpace(nullptr);
        }
        m_active = runnable;
        emit activeChanged(runnable);
        if ( m_active ){
            m_active->setRunSpace(m_runspace);
            m_programHolder->setMain(m_active->program());
        }

        scheduleRun();
    }
}

ProjectDocument *Project::createTextDocument(ProjectFile *file, const QString& format, bool isMonitored){
    ProjectDocument* document = new ProjectDocument(file->path(), format, isMonitored, this);
    file->setDocument(document);

    connect(document->textDocument(), &QTextDocument::contentsChange, [this, document](int, int, int){

        if ( m_excludedRunTriggers.contains(document->path()) )
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

Document *Project::createDocument(ProjectFile *file, const QString &formatType, bool isMonitored){
    Document* document = new Document(file->path(), formatType, isMonitored, this);
    file->setDocument(document);
    emit documentOpened(document);
    return document;
}

void Project::documentSaved(Document *document){
    if ( m_runTrigger == Project::RunOnSave ){
        scheduleRun();
    }
    emit fileChanged(document->path());
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
