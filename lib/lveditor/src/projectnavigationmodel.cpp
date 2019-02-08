/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
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

#include "projectnavigationmodel.h"
#include "live/projectdocumentmodel.h"
#include "live/projectdocument.h"
#include "live/projectfile.h"
#include "live/project.h"

#include <QtConcurrent/QtConcurrent>
#include <QDirIterator>

namespace lv{

ProjectNavigationModel::ProjectNavigationModel(Project *project)
    : QAbstractListModel(project)
    , m_isIndexing(false)
    , m_requiresReindex(true)
    , m_project(project)
{
    m_roles[ProjectNavigationModel::Name]   = "name";
    m_roles[ProjectNavigationModel::Path]   = "path";
    m_roles[ProjectNavigationModel::IsOpen] = "isOpen";

    connect(&m_workerWatcher, SIGNAL(finished()), this, SLOT(reindexReady()));
}

ProjectNavigationModel::~ProjectNavigationModel(){
}

int ProjectNavigationModel::rowCount(const QModelIndex &) const{
    return m_filteredOpenedFiles.size() + m_filteredFiles.size();
}

QVariant ProjectNavigationModel::data(const QModelIndex &index, int role) const{
    int row = index.row();

    if ( row < m_filteredOpenedFiles.size() ){
        if ( role == ProjectNavigationModel::Name ){
            return m_filteredOpenedFiles[row].name;
        } else if ( role == ProjectNavigationModel::Path ){
            return m_filteredOpenedFiles[row].path;
        } else if ( role == ProjectNavigationModel::IsOpen ){
            return true;
        }
    } else {
        row = row - m_filteredOpenedFiles.size();
        int fileIndex = m_filteredFiles[row];
        if ( role == ProjectNavigationModel::Name ){
            return m_files[fileIndex].name;
        } else if ( role == ProjectNavigationModel::Path ){
            return m_files[fileIndex].path;
        } else if ( role == ProjectNavigationModel::IsOpen ){
            return false;
        }
    }
    return QVariant();
}

void ProjectNavigationModel::reindex(){
    if ( m_requiresReindex && m_project->rootPath() != "" ){
        beginResetModel();
        m_filteredFiles.clear();
        m_files.clear();
        endResetModel();

        beginIndexing();
        m_requiresReindex = false;
        QFuture<QList<ProjectNavigationModel::Entry> > future = QtConcurrent::run(
            [](QString path){

                QList<ProjectNavigationModel::Entry> entries;

                QDirIterator dit(path, QDirIterator::Subdirectories);
                while( dit.hasNext() ){
                    dit.next();
                    QFileInfo info = dit.fileInfo();
                    if ( info.fileName() == "." || info.fileName() == ".." || info.isDir() )
                        continue;

                    entries.append(ProjectNavigationModel::Entry(info.fileName(), info.filePath()));
                }

                return entries;
        }, m_project->rootPath());

        m_workerWatcher.setFuture(future);
    }
}

void ProjectNavigationModel::reindexReady(){
    beginResetModel();
    m_files = m_workerWatcher.result();
    updateFilters();
    endResetModel();
    m_requiresReindex = false;
    endIndexing();
}

void ProjectNavigationModel::requiresReindex(){
    m_requiresReindex = true;
}

void ProjectNavigationModel::updateFilters(){
    m_filteredOpenedFiles.clear();
    ProjectDocumentModel* dm = m_project->documentModel();
    for( QHash<QString, ProjectDocument*>::const_iterator it = dm->openedFiles().begin();
         it != dm->openedFiles().end();
         ++it )
    {
        ProjectFile* file = it.value()->file();
        if ( m_filter.size() <= file->name().size() )
            if ( file->name().contains(m_filter, Qt::CaseInsensitive) )
                m_filteredOpenedFiles.append(ProjectNavigationModel::Entry(
                    file->name(), file->path()
                ));
    }

    m_filteredFiles.clear();
    for ( int i = 0; i < m_files.size(); ++i ){
        if ( m_filter.size() <= m_files[i].name.size() )
            if ( m_files[i].name.contains(m_filter, Qt::CaseInsensitive) &&
                 !m_project->isOpened(m_files[i].path))
                m_filteredFiles.append(i);
    }
}

void ProjectNavigationModel::setFilter(const QString &filter){
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

void ProjectNavigationModel::directoryChanged(const QString &){
    requiresReindex();
}


}// namespace
