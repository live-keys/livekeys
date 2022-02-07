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

#include "projectfileindexer.h"
#include "live/projectdocumentmodel.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "live/project.h"

#include <QtConcurrent/QtConcurrent>
#include <QDirIterator>

namespace lv{

ProjectFileIndexer::ProjectFileIndexer(Project *project)
    : QAbstractListModel(project)
    , m_isIndexing(false)
    , m_requiresReindex(true)
    , m_project(project)
    , m_projectDocuments(nullptr)
{
    m_roles[ProjectFileIndexer::Name]   = "name";
    m_roles[ProjectFileIndexer::Path]   = "path";
    m_roles[ProjectFileIndexer::IsOpen] = "isOpen";

    connect(&m_workerWatcher, SIGNAL(finished()), this, SLOT(__reindexReady()));
}

ProjectFileIndexer::~ProjectFileIndexer(){
}

int ProjectFileIndexer::rowCount(const QModelIndex &) const{
    return m_filteredOpenedFiles.size() + m_filteredFiles.size();
}

QVariant ProjectFileIndexer::data(const QModelIndex &index, int role) const{
    int row = index.row();

    if ( row < m_filteredOpenedFiles.size() ){
        if ( role == ProjectFileIndexer::Name ){
            return m_filteredOpenedFiles[row].name;
        } else if ( role == ProjectFileIndexer::Path ){
            return m_filteredOpenedFiles[row].path;
        } else if ( role == ProjectFileIndexer::IsOpen ){
            return true;
        }
    } else {
        row = row - m_filteredOpenedFiles.size();
        int fileIndex = m_filteredFiles[row];
        if ( role == ProjectFileIndexer::Name ){
            return m_files[fileIndex].name;
        } else if ( role == ProjectFileIndexer::Path ){
            return m_files[fileIndex].path;
        } else if ( role == ProjectFileIndexer::IsOpen ){
            return false;
        }
    }
    return QVariant();
}

void ProjectFileIndexer::filter(const QJSValue &options){
    m_projectDocuments = qobject_cast<ProjectDocumentModel*>(options.property("documents").toQObject());

    QString f = options.hasOwnProperty("contains") ? options.property("contains").toString() : "";

    if ( m_filter != f ){
        m_filter = f;
        if ( !m_isIndexing ){
            if ( m_requiresReindex ){
                reindex();
            } else {
                beginResetModel();
                updateFilters();
                endResetModel();
            }
        }
    }
}

void ProjectFileIndexer::reindex(){
    if ( m_requiresReindex && m_project->rootPath() != "" ){
        beginResetModel();
        m_filteredFiles.clear();
        m_files.clear();
        endResetModel();

        beginIndexing();
        m_requiresReindex = false;
        QFuture<QList<ProjectFileIndexer::Entry> > future = QtConcurrent::run(
            [](QString path){

                QList<ProjectFileIndexer::Entry> entries;

                QDirIterator dit(path, QDirIterator::Subdirectories);
                while( dit.hasNext() ){
                    dit.next();
                    QFileInfo info = dit.fileInfo();
                    if ( info.fileName() == "." || info.fileName() == ".." || info.isDir() )
                        continue;

                    entries.append(ProjectFileIndexer::Entry(info.fileName(), info.filePath()));
                }

                return entries;
        }, m_project->rootPath());

        m_workerWatcher.setFuture(future);
    }
}

void ProjectFileIndexer::__reindexReady(){
    beginResetModel();
    m_files = m_workerWatcher.result();
    updateFilters();
    endResetModel();
    m_requiresReindex = false;
    endIndexing();
}

void ProjectFileIndexer::requiresReindex(){
    m_requiresReindex = true;
}

void ProjectFileIndexer::updateFilters(){
    m_filteredOpenedFiles.clear();
    ProjectDocumentModel* dm = m_projectDocuments;
    if ( dm ){
        for( auto it = dm->openedFiles().begin(); it != dm->openedFiles().end(); ++it ){
            Document* doc = it.value();
            if ( m_filter.size() <= doc->fileName().size() ){
                if ( doc->fileName().contains(m_filter, Qt::CaseInsensitive) ){
                    m_filteredOpenedFiles.append(ProjectFileIndexer::Entry(
                        doc->fileName(), doc->path()
                    ));
                }
            }
        }
    }

    m_filteredFiles.clear();
    for ( int i = 0; i < m_files.size(); ++i ){
        if ( m_filter.size() <= m_files[i].name.size() ){
            if ( m_files[i].name.contains(m_filter, Qt::CaseInsensitive) ){
                if ( dm ){
                    if ( !m_project->isOpened(m_files[i].path ))
                        m_filteredFiles.append(i);
                } else {
                    m_filteredFiles.append(i);
                }
            }
        }
    }
}

void ProjectFileIndexer::setFilter(const QString &filter){
    if ( m_filter != filter ){
        m_filter = filter;
        if ( !m_isIndexing ){
            if ( m_requiresReindex ){
                reindex();
            } else {
                beginResetModel();
                updateFilters();
                endResetModel();
            }

        }
    }
}

void ProjectFileIndexer::__directoryChanged(const QString &){
    requiresReindex();
}


}// namespace
