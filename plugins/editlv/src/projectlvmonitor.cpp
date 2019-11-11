#include "projectlvmonitor.h"
#include "projectlvscanner.h"
#include "languagelvextension.h"

#include "live/project.h"
#include "live/viewengine.h"
#include "live/visuallog.h"
#include "live/exception.h"

namespace lv{

ProjectLvMonitor::ProjectLvMonitor(
        LanguageLvExtension *extension, Project *project, ViewEngine *engine, QObject *parent)
    : QObject(parent)
    , m_projectExtension(extension)
    , m_scanner(new ProjectLvScanner)
    , m_project(project)
    , m_engine(engine)
{
//    connect(project, &Project::pathChanged, this, &ProjectLvMonitor::__newProject);
//    connect(project, &Project::directoryChanged, this, &ProjectLvMonitor::__directoryChanged);
//    connect(project, &Project::fileChanged, this, &ProjectLvMonitor::__fileChanged);
}

ProjectLvMonitor::~ProjectLvMonitor(){
    delete m_scanner;
}

void ProjectLvMonitor::__newProject(const QString &){
    //TODO: Reset scanner
    delete m_scanner;

}

void ProjectLvMonitor::__directoryChanged(const QString &){
    //TODO: scanner->resetLibrariesInPath(path);
}

void ProjectLvMonitor::__fileChanged(const QString &){
    //TODO: scanner->resetFile(path);
}

}// namespace
