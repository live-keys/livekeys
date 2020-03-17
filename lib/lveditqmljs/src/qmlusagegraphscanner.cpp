#include "qmlusagegraphscanner.h"

#include "live/project.h"
#include "live/projectdocument.h"
#include "live/runnablecontainer.h"
#include "live/visuallogqt.h"
#include "live/documentqmlscope.h"

namespace lv{

QmlUsageGraphScanner::QmlUsageGraphScanner(Project *project, const QmlScopeSnap& qss, QObject *parent)
    : QThread(parent)
    , m_stopRequest(false)
    , m_project(project)
    , m_scopeSnap(qss)
{
    RunnableContainer* rc = project->runnables();

    int totalRunnables = rc->rowCount();
    for ( int i = 0; i < totalRunnables; ++i ){
        Runnable* r = rc->runnableAt(i);
        if ( !r->name().isEmpty() && r->type() == Runnable::QmlFile ){

            vlog("editqmljs-usagegraph").v() << "Adding runnable:" << r->name();

            if ( r->name().front().isLower() ){
                ProjectDocument* document = ProjectDocument::castFrom(m_project->isOpened(r->path()));
                if ( document ){
                    m_runnables.insert(r->path(), document->content());
                }
            }
        }
    }
}

void QmlUsageGraphScanner::followGraph(
        const QString& component,
        const QString& componentPath,
        const QList<QmlUsageGraphScanner::BindingEntry>& gatheredComponents,
        const QMap<QString, QList<QmlBindingPath::Ptr> >& usageGraph)
{
    if ( m_stopRequest )
        return;

    vlog("editqmljs-usagegraph").v() << "Following component: " << component;

    auto usageIt = usageGraph.find(component);
    if ( usageIt == usageGraph.end() )
        return;

    // usageIt now shows the references where the 'key' is used
    QList<QmlBindingPath::Ptr> keyUsage = usageIt.value();

    for ( auto refIt = keyUsage.begin(); refIt != keyUsage.end(); ++refIt ){

        const QmlBindingPath::Ptr& bp = *refIt;
        QString filePath = bp->rootFile();
        if ( !filePath.isEmpty() ){
            // for each reference, if the reference matches a runnable, then push the result

            QString fileName = filePath.mid(filePath.lastIndexOf('/') + 1);

            if ( m_runnables.contains(filePath) ){
                QmlUsageGraphScanner::BindingEntry be;
                be.componentName = component;
                be.componentPath = componentPath;
                be.path          = bp;

                pushGraphResult(QList<QmlUsageGraphScanner::BindingEntry>() << gatheredComponents << be);
            }

            if ( !fileName.isEmpty() && fileName.front().isUpper() ){
                QmlUsageGraphScanner::BindingEntry be;
                be.componentName = component;
                be.componentPath = componentPath;
                be.path          = bp;

                followGraph(
                    fileName.mid(0, fileName.indexOf('.')),
                    filePath,
                    QList<QmlUsageGraphScanner::BindingEntry>() << gatheredComponents << be,
                    usageGraph
                );
            }
        }
    }
}

void QmlUsageGraphScanner::pushGraphResult(QList<QmlUsageGraphScanner::BindingEntry> segments){
    if ( segments.isEmpty() )
        return;

    for ( auto it = segments.begin(); it != segments.end(); ++it ){
        const QmlUsageGraphScanner::BindingEntry& be = *it;
        if ( !checkEntry(be) ){
            vlog("editqmljs-usagegraph").v() << "Entry is not valid: " << be.toString();
            return;
        } else {
            vlog("editqmljs-usagegraph").v() << "Entry passed: " << be.toString();
        }
    }

    // append the bindings in reverse, last segment is the runnable
    QmlBindingPath::Ptr result(nullptr);
    for ( auto it = segments.begin(); it != segments.end(); ++it ){
        const QmlUsageGraphScanner::BindingEntry& be = *it;
        result = result ? QmlBindingPath::join(be.path, result) : be.path;
    }

    if ( result ){
        vlog("editqmljs-usagegraph").v() << "Result: Runnable \'" << segments.last().componentName << "\' binding path: " << result->toString();
        m_scannedResults.append(result);
        emit bindingPathAdded();
    }

}

QPair<Runnable *, QmlBindingPath::Ptr> QmlUsageGraphScanner::popResult(){
    QmlBindingPath::Ptr rbp = m_scannedResults.front();
    m_scannedResults.pop_front();

    RunnableContainer* rc = m_project->runnables();
    return QPair<Runnable*, QmlBindingPath::Ptr>(rc->runnableAt(rbp->rootFile()), rbp);
}

bool QmlUsageGraphScanner::checkEntry(const QmlUsageGraphScanner::BindingEntry &entry){
    QString file = entry.path->rootFile();
    if ( file.isEmpty() )
        return false;

    QString content = m_runnables.contains(file)
            ? m_runnables[file]
            : QString::fromStdString(m_project->lockedFileIO()->readFromFile(file.toStdString()));

    ProjectQmlScope::Ptr projectScope = m_scopeSnap.project;

    DocumentQmlInfo::Ptr documentInfo = DocumentQmlInfo::create(file);
    if ( documentInfo->parse(content) ){
        QList<DocumentQmlInfo::Import> imports = m_scopeSnap.document->info()->imports();
        for ( auto it = imports.begin(); it != imports.end(); ++it ){
            DocumentQmlInfo::Import& imp = *it;

            if ( imp.importType() == DocumentQmlInfo::Import::Directory ){
                // find library by path
                QmlLibraryInfo::Ptr qli = projectScope->globalLibraries()->libraryInfo(imp.uri());
                if ( !qli )
                    qli = projectScope->implicitLibraries()->libraryInfo(imp.uri());

                if ( qli ){
                    QStringList exports;
                    qli->listExports(&exports);

                    QmlLibraryInfo::ExportVersion ev = qli->findExport(entry.componentName);
                    if ( ev.isValid() ){
                        if ( entry.componentPath.startsWith(imp.uri())){
                            return true;
                        }
                    }
                }

            } else if ( imp.importType() == DocumentQmlInfo::Import::Library ){
                // find library by uri
                QmlLibraryInfo::Reference pqli = projectScope->globalLibraries()->libraryInfoByNamespace(imp.uri());
                QmlLibraryInfo::Ptr qli = pqli.lib;
                if ( qli ){
                    QmlLibraryInfo::ExportVersion ev = qli->findExport(entry.componentName);
                    if ( ev.isValid() ){
                        if ( entry.componentPath.startsWith(pqli.path)){
                            return true;
                        }
                    }
                }
            }
        }

        // check library in the same folder
        QString implicitPath = file.mid(0, file.lastIndexOf('/'));
        QmlLibraryInfo::Ptr qli = projectScope->implicitLibraries()->libraryInfo(implicitPath);
        if ( qli ){
            QmlLibraryInfo::ExportVersion ev = qli->findExport(entry.componentName);
            if ( ev.isValid() ){
                return true;
            }
        }
    }

    return false;
}

void QmlUsageGraphScanner::run(){
    if (m_searchComponent.isEmpty() || m_searchComponentFile.isEmpty() )
        return;

    QmlScopeSnap& scope = m_scopeSnap;

    vlog("editqmljs-usagegraph").v() <<
        "Number of implicit libraries: " << scope.project->implicitLibraries()->totalLibraries();

    ProjectQmlScopeContainer* implicits = scope.project->implicitLibraries();
    ProjectQmlScopeContainer* globalLibs = scope.project->globalLibraries();
    auto libs = globalLibs->getLibrariesInPath(m_project->dir());

    auto implicitLibs = implicits->getLibrariesInPath(m_project->dir());
    for ( auto lit = implicitLibs.begin(); lit != implicitLibs.end(); ++lit){
        libs.insert(lit.key(), lit.value());
    }

    vlog("editqmljs-usagegraph").v() << "Number of libraries to scan: " << libs.size();

    QMap<QString, QList<QmlBindingPath::Ptr> > usageGraph;

    for ( auto it = libs.begin(); it != libs.end(); ++it ){
        QmlJS::LibraryInfo linfo = it.value()->data();
        auto linfoc = linfo.components();

        auto exports = it.value()->exports();

        vlog("editqmljs-usagegraph").v() << "Library \'" << it.key() << "\' total exports: " << exports.size();

        QMap<QString, QString> scannedFiles =  it.value()->files();
        for ( auto fit = scannedFiles.begin(); fit != scannedFiles.end(); ++fit ){
            QString filePath = it.key() + "/" + fit.value();

            std::string content = m_project->lockedFileIO()->readFromFile(filePath.toStdString());
            QString qcontent = QString::fromStdString(content);

            DocumentQmlInfo::Ptr documentInfo = DocumentQmlInfo::create(filePath);
            if ( documentInfo->parse(qcontent) ){
                lv::DocumentQmlValueObjects::Ptr objects = documentInfo->createObjects();
                lv::DocumentQmlValueObjects::RangeObject* root = objects->root();

                QmlBindingPath::Ptr bp = QmlBindingPath::create();
                bp->appendFile(filePath);
                bp->appendComponent(documentInfo->componentName(), it.value()->importNamespace());
                bp->appendIndex(0);
                QList<QmlUsageGraphScanner::BindingEntry> objectDeclarations = extractRanges(root, bp, qcontent, filePath);

                for ( const QmlUsageGraphScanner::BindingEntry& od: objectDeclarations ){
                    auto usageIt = usageGraph.find(od.componentName);
                    if ( usageIt == usageGraph.end() ){
                        usageGraph.insert(od.componentName, QList<QmlBindingPath::Ptr>() << od.path);
                    } else {
                        usageIt.value().append(od.path);
                    }
                }
            }

            if ( m_stopRequest )
                return;
        }

        if ( m_stopRequest )
            return;
    }

    // Store runnables as a map of string and content

    for ( auto it = m_runnables.begin(); it != m_runnables.end(); ++it ){
        DocumentQmlInfo::Ptr documentInfo = DocumentQmlInfo::create(it.key());
        if ( documentInfo->parse(it.value()) ){
            lv::DocumentQmlValueObjects::Ptr objects = documentInfo->createObjects();
            lv::DocumentQmlValueObjects::RangeObject* root = objects->root();

            QmlBindingPath::Ptr bp = QmlBindingPath::create();
            bp->appendFile(it.key());
            bp->appendIndex(0);
            QList<QmlUsageGraphScanner::BindingEntry> objectDeclarations = extractRanges(root, bp, it.value(), it.key());

            for ( const QmlUsageGraphScanner::BindingEntry& od: objectDeclarations ){
                auto usageIt = usageGraph.find(od.componentName);
                if ( usageIt == usageGraph.end() ){
                    usageGraph[od.componentName] = (QList<QmlBindingPath::Ptr>() << od.path);
                } else {
                    usageIt.value().append(od.path);
                }
            }
        }
        if ( m_stopRequest )
            return;
    }

    vlog("editqmljs-usagegraph").v() << "Total usage graph keys: " << usageGraph.size();

    followGraph(m_searchComponent, m_searchComponentFile, QList<QmlUsageGraphScanner::BindingEntry>(), usageGraph);
}

void QmlUsageGraphScanner::setSearchComponent(const QString componentFile, const QString &component){
    m_searchComponent = component;
    m_searchComponentFile = componentFile;
}

QList<QmlUsageGraphScanner::BindingEntry> QmlUsageGraphScanner::extractRanges(
        DocumentQmlValueObjects::RangeObject *obj, QmlBindingPath::Ptr bp, const QString &source, const QString &file)
{
    QList<QmlUsageGraphScanner::BindingEntry> refs;
    if ( obj ){
        QString objectIdentifier = source.mid(obj->begin, obj->identifierEnd - obj->begin);

        QmlUsageGraphScanner::BindingEntry r;
        r.componentName = objectIdentifier;
        r.path = bp->clone();
        refs.append(r);

        for ( int i = 0; i < obj->children.size(); ++i ){
            QmlBindingPath::Ptr objectBp = bp->clone();
            objectBp->appendIndex(i);
            refs.append(extractRanges(obj->children[i], objectBp, source, file));
        }

        for ( auto it = obj->properties.begin(); it != obj->properties.end(); ++it ){
            DocumentQmlValueObjects::RangeProperty* prop = *it;

            if ( prop->child ){
                QmlBindingPath::Ptr propBp = bp->clone();
                // Extract property name

                QStringList propName = prop->name();
                for ( const QString& str : propName ){
                    propBp->appendProperty(str, objectIdentifier.split("."));
                }

                refs.append(extractRanges(prop->child, bp, source, file));
            }
        }
    }

    return refs;
}

}// namespace
