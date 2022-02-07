/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#include "qmlusagegraphscanner.h"

#include "live/project.h"
#include "live/projectdocument.h"
#include "live/runnablecontainer.h"
#include "live/viewengine.h"
#include "live/visuallogqt.h"

namespace lv{

QmlUsageGraphScanner::QmlUsageGraphScanner(ViewEngine* engine, Project *project, const QmlScopeSnap& qss, QObject *parent)
    : QThread(parent)
    , m_stopRequest(false)
    , m_viewEngine(engine)
    , m_project(project)
    , m_scopeSnap(qss)
    , m_scannedResults(new std::list<QmlBindingPath::Ptr>)
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

QmlUsageGraphScanner::~QmlUsageGraphScanner(){
    delete m_scannedResults;
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
        m_scannedResults->push_back(result);
        emit bindingPathAdded();
    }

}

QPair<Runnable *, QmlBindingPath::Ptr> QmlUsageGraphScanner::popResult(){
    QmlBindingPath::Ptr rbp = m_scannedResults->front();
    m_scannedResults->pop_front();

    RunnableContainer* rc = m_project->runnables();
    return QPair<Runnable*, QmlBindingPath::Ptr>(rc->runnableAt(rbp->rootFile()), rbp);
}

bool QmlUsageGraphScanner::checkEntry(const QmlUsageGraphScanner::BindingEntry &entry){
    QString file = entry.path->rootFile();
    if ( file.isEmpty() )
        return false;

    QString content = m_runnables.contains(file)
            ? m_runnables[file]
            : QString::fromStdString(m_viewEngine->fileIO()->readFromFile(file.toStdString()));

    ProjectQmlScope::Ptr projectScope = m_scopeSnap.project;

    DocumentQmlInfo::Ptr documentInfo = DocumentQmlInfo::create(file);
    if ( documentInfo->parse(content) ){
        QList<DocumentQmlInfo::Import> imports = m_scopeSnap.document->imports();
        for ( auto it = imports.begin(); it != imports.end(); ++it ){
            DocumentQmlInfo::Import& imp = *it;

            if ( imp.importType() == DocumentQmlInfo::Import::Directory ){
                // find library by path
                QmlLibraryInfo::Ptr qli = projectScope->libraryInfo(imp.uri());
                if ( !qli )
                    qli = projectScope->libraryInfo(imp.uri());

                if ( qli ){
                    QmlTypeInfo::Ptr ti = qli->typeInfoByName(entry.componentName);
                    if ( ti ){
                        if ( entry.componentPath.startsWith(imp.uri())){
                            return true;
                        }
                    }
                }

            } else if ( imp.importType() == DocumentQmlInfo::Import::Library ){
                // find library by uri
                QmlLibraryInfo::Ptr qli = projectScope->libraryInfo(imp.uri());
                if ( qli ){
                    QmlTypeInfo::Ptr ti = qli->typeInfoByName(entry.componentName);
                    if ( ti ){
                        if ( entry.componentPath.startsWith(qli->path())){
                            return true;
                        }
                    }
                }
            }
        }

        // check library in the same folder
        QString implicitPath = file.mid(0, file.lastIndexOf('/'));
        QmlLibraryInfo::Ptr qli = projectScope->libraryInfo(implicitPath);
        if ( qli ){
            QmlTypeInfo::Ptr ti = qli->typeInfoByName(entry.componentName);
            if ( ti ){
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

    auto libs = scope.project->getLibrariesInPath(m_project->dir());

    vlog("editqmljs-usagegraph").v() << "Number of libraries to scan: " << libs.size();

    QMap<QString, QList<QmlBindingPath::Ptr> > usageGraph;

    for ( auto it = libs.begin(); it != libs.end(); ++it ){

        if ( m_stopRequest )
            return;

        QmlLibraryInfo::ConstPtr linfo = *it;
        int totalExports = 0;
        for ( auto eit = linfo->exports().begin(); eit != linfo->exports().end(); ++eit ){
            const QmlTypeInfo::Ptr& ti = eit.value();
            if ( ti->document().isValid() ){
                QString filePath = ti->document().path;

                std::string content = m_viewEngine->fileIO()->readFromFile(filePath.toStdString());
                QString qcontent = QString::fromStdString(content);

                DocumentQmlInfo::Ptr documentInfo = DocumentQmlInfo::create(filePath);
                if ( documentInfo->parse(qcontent) ){
                    lv::DocumentQmlValueObjects::Ptr objects = documentInfo->createObjects();
                    lv::DocumentQmlValueObjects::RangeObject* root = objects->root();

                    QmlBindingPath::Ptr bp = QmlBindingPath::create();
                    bp->appendFile(filePath);
                    bp->appendComponent(documentInfo->componentName(), (*it)->uri());
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

                ++totalExports;
            }
        }

        vlog("editqmljs-usagegraph").v() << "Library \'" << (*it)->uri() << "\' total exports: " << totalExports;
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
