#include "qprojectnavigationmodel.h"
#include "qprojectdocumentmodel.h"
#include "qprojectdocument.h"
#include "qprojectfile.h"
#include "qproject.h"

#include <QtConcurrent/QtConcurrent>
#include <QDirIterator>

namespace lcv{

QProjectNavigationModel::QProjectNavigationModel(QProject *project)
    : QAbstractListModel(project)
    , m_project(project)
    , m_isIndexing(false)
    , m_requiresReindex(true)
{
    m_roles[QProjectNavigationModel::Name]   = "name";
    m_roles[QProjectNavigationModel::Path]   = "path";
    m_roles[QProjectNavigationModel::IsOpen] = "isOpen";

    connect(&m_workerWatcher, SIGNAL(finished()), this, SLOT(reindexReady()));
}

QProjectNavigationModel::~QProjectNavigationModel(){
}

int QProjectNavigationModel::rowCount(const QModelIndex &) const{
    return m_filteredOpenedFiles.size() + m_filteredFiles.size();
}

QVariant QProjectNavigationModel::data(const QModelIndex &index, int role) const{
    int row = index.row();

    if ( row < m_filteredOpenedFiles.size() ){
        if ( role == QProjectNavigationModel::Name ){
            return m_filteredOpenedFiles[row].name;
        } else if ( role == QProjectNavigationModel::Path ){
            return m_filteredOpenedFiles[row].path;
        } else if ( role == QProjectNavigationModel::IsOpen ){
            return true;
        }
    } else {
        row = row - m_filteredOpenedFiles.size();
        int fileIndex = m_filteredFiles[row];
        if ( role == QProjectNavigationModel::Name ){
            return m_files[fileIndex].name;
        } else if ( role == QProjectNavigationModel::Path ){
            return m_files[fileIndex].path;
        } else if ( role == QProjectNavigationModel::IsOpen ){
            return false;
        }
    }
    return QVariant();
}

void QProjectNavigationModel::reindex(){
    qDebug() << "Project path:" << m_project->path();
    if ( m_requiresReindex ){
        beginResetModel();
        m_filteredFiles.clear();
        m_files.clear();
        endResetModel();

        beginIndexing();
        m_requiresReindex = false;
        QFuture<QList<QProjectNavigationModel::Entry> > future = QtConcurrent::run(
            [](QString path){

                QList<QProjectNavigationModel::Entry> entries;

                QDirIterator dit(path, QDirIterator::Subdirectories);
                while( dit.hasNext() ){
                    dit.next();
                    QFileInfo info = dit.fileInfo();
                    if ( info.fileName() == "." || info.fileName() == ".." || info.isDir() )
                        continue;

                    entries.append(QProjectNavigationModel::Entry(info.fileName(), info.filePath()));
                }

                return entries;
        }, m_project->path());

        m_workerWatcher.setFuture(future);
    }
}

void QProjectNavigationModel::reindexReady(){
    beginResetModel();
    m_files = m_workerWatcher.result();
    updateFilters();
    endResetModel();
    m_requiresReindex = false;
    endIndexing();
}

void QProjectNavigationModel::requiresReindex(){
    m_requiresReindex = true;
}

void QProjectNavigationModel::updateFilters(){
    m_filteredOpenedFiles.clear();
    QProjectDocumentModel* dm = m_project->documentModel();
    for( QHash<QString, QProjectDocument*>::const_iterator it = dm->openedFiles().begin();
         it != dm->openedFiles().end();
         ++it )
    {
        QProjectFile* file = it.value()->file();
        if ( m_filter.size() <= file->name().size() )
            if ( file->name().contains(m_filter, Qt::CaseInsensitive) )
                m_filteredOpenedFiles.append(QProjectNavigationModel::Entry(
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

void QProjectNavigationModel::setFilter(const QString &filter){
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

void QProjectNavigationModel::directoryChanged(const QString &){
    requiresReindex();
}


}// namespace
