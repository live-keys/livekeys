#include "runnablecontainer.h"
#include "runnable.h"
#include "project.h"

namespace lv{

RunnableContainer::RunnableContainer(Project *parent)
    : QAbstractListModel(parent)
{
    m_roles[RunnableContainer::Name] = "name";
    m_roles[RunnableContainer::Path] = "path";

    updateFilters();
}

RunnableContainer::~RunnableContainer(){
}

int RunnableContainer::rowCount(const QModelIndex &) const{
    return m_filteredRunnables.size();
}

QVariant RunnableContainer::data(const QModelIndex &index, int role) const{
    int row = index.row();
    if ( row < 0 || row >= m_filteredRunnables.size() )
        return QVariant();

    Runnable* sp = m_runnables[m_filteredRunnables[index.row()]];

    if ( role == RunnableContainer::Name ){
        return sp->name();
    } else if ( role == RunnableContainer::Path )
        return sp->path();

    return QVariant();
}

void RunnableContainer::addRunnable(Runnable *runnable){
    beginResetModel();
    m_runnables.append(runnable);
    updateFilters();
    endResetModel();
}

void RunnableContainer::closeRunnable(Runnable *runnable){
    beginResetModel();
    m_runnables.removeAll(runnable);
    delete runnable;
    updateFilters();
    endResetModel();
}

void RunnableContainer::clearAll(){
    beginResetModel();
    m_filteredRunnables.clear();
    for ( auto it = m_runnables.begin(); it != m_runnables.end(); ++it ){
        delete *it;
    }
    m_runnables.clear();
    endResetModel();
}

Runnable *RunnableContainer::runnableAt(const QString &path){
    for ( auto it = m_runnables.begin(); it != m_runnables.end(); ++it ){
        if ( (*it)->path() == path )
            return *it;
    }
    return nullptr;
}

QList<Runnable *> RunnableContainer::runnablesInPath(const QString &path){
    QList<Runnable *> result;
    for ( auto it = m_runnables.begin(); it != m_runnables.end(); ++it ){
        if ( (*it)->path().startsWith(path) )
            result.append(*it);
    }
    return result;
}

void RunnableContainer::setFilter(const QString &filter){
    if ( m_filter != filter ){
        m_filter = filter;
        beginResetModel();
        updateFilters();
        endResetModel();
    }
}

void RunnableContainer::updateFilters(){
    m_filteredRunnables.clear();
    if ( m_filter.isEmpty() ){
        for ( int i = 0; i < m_runnables.size(); ++i )
            m_filteredRunnables.append(i);
    } else {
        for ( int i = 0; i < m_runnables.size(); ++i ){
            Runnable* pr = m_runnables[i];
            if ( pr->name().contains(m_filter, Qt::CaseInsensitive ) ){
                m_filteredRunnables.append(i);
            }
        }
    }
}

}// namespace
