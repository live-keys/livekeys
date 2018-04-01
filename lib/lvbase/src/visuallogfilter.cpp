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

#include "visuallogfilter.h"
#include "live/visuallog.h"
#include "live/engine.h"

#include <QQmlEngine>
#include <QJSEngine>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>

namespace lv{

// SearchQuery
// ---------------------------------------------------------------

VisualLogFilter::SearchQuery::SearchQuery(const VisualLogFilter::SearchQuery &other)
    : m_type(other.m_type)
    , m_container(0)
{
    if ( m_type == SearchQuery::Regexp ){
        m_container = new SearchContainer(*other.m_container->searchRegexp);
    } else if ( m_type == SearchQuery::String ){
        m_container = new SearchContainer(*other.m_container->searchString);
    }
}

VisualLogFilter::SearchQuery::SearchQuery(const QJSValue &value, QJSEngine *engine)
    : m_type(SearchQuery::None)
    , m_container(0)
{
    if ( value.isRegExp() ){
        m_type = SearchQuery::Regexp;
        m_container = new SearchContainer(engine->fromScriptValue<QRegExp>(value));
    } else if ( value.isString() && !value.toString().isEmpty() ){
        m_type = SearchQuery::String;
        m_container = new SearchContainer(value.toString());
    }
}

VisualLogFilter::SearchQuery::~SearchQuery(){
    if ( m_type == SearchQuery::Regexp )
        delete m_container->searchRegexp;
    else if ( m_type == SearchQuery::String )
        delete m_container->searchString;

    delete m_container;
}

VisualLogFilter::SearchQuery &VisualLogFilter::SearchQuery::operator =(const VisualLogFilter::SearchQuery& other){
    m_type = other.m_type;
    if ( m_type == SearchQuery::Regexp ){
        delete m_container;
        m_container = new SearchContainer(*other.m_container->searchRegexp);
    } else if ( m_type == SearchQuery::String ){
        delete m_container;
        m_container = new SearchContainer(*other.m_container->searchString);
    }
    return *this;
}

bool VisualLogFilter::SearchQuery::operator ==(const VisualLogFilter::SearchQuery &other){
    if ( other.m_type != m_type )
        return false;
    if ( m_type == SearchQuery::Regexp )
        return m_container->searchRegexp == other.m_container->searchRegexp;
    else if ( m_type == SearchQuery::String )
        return m_container->searchString == other.m_container->searchString;
    else
        return true;
}

QJSValue VisualLogFilter::SearchQuery::toJs(QJSEngine *engine) const{
    if ( m_type == SearchQuery::Regexp ){
        return engine->toScriptValue(*m_container->searchRegexp);
    } else if ( m_type == SearchQuery::String ){
        return QJSValue(*m_container->searchString);
    } else {
        return QJSValue();
    }
}

int VisualLogFilter::SearchQuery::locateIn(const QString &str){
    if ( m_type == SearchQuery::String ){
        return str.indexOf(m_container->searchString);
    } else if ( m_type == SearchQuery::Regexp ){
        return m_container->searchRegexp->indexIn(str);
    } else {
        return -1;
    }
}

// VisualLogFilter
// ---------------------------------------------------------------

VisualLogFilter::VisualLogFilter(QObject *parent)
    : VisualLogBaseModel(parent)
    , m_source(0)
    , m_componentReady(false)
    , m_isIndexing(false)
    , m_workerIgnoreResult(false)
{
    connect(&m_workerWatcher, SIGNAL(finished()), this, SLOT(refilterReady()));
}

VisualLogFilter::~VisualLogFilter(){
}

void VisualLogFilter::setSource(VisualLogBaseModel *source){
    if (m_source == source)
        return;

    m_workerWatcher.waitForFinished();

    if ( m_source ){
        disconnect(m_source, SIGNAL(destroyed(QObject*)),
                   this,     SLOT(sourceDestroyed()));
        disconnect(m_source, SIGNAL(modelAboutToBeReset()),
                   this,     SLOT(sourceModelAboutToReset()));
        disconnect(m_source, SIGNAL(modelReset()),
                   this,     SLOT(sourceModelReset()));
        disconnect(m_source, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                   this,     SLOT(sourceRowsAboutToBeRemoved(QModelIndex,int,int)));
        disconnect(m_source, SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this,     SLOT(sourceRowsInserted(QModelIndex,int,int)));
    }

    m_source = source;

    if ( m_source ){
        connect(m_source, SIGNAL(destroyed(QObject*)),
                   this,     SLOT(sourceDestroyed()));
        connect(m_source, SIGNAL(modelAboutToBeReset()),
                   this,     SLOT(sourceModelAboutToReset()));
        connect(m_source, SIGNAL(modelReset()),
                   this,     SLOT(sourceModelReset()));
        connect(m_source, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
                   this,     SLOT(sourceRowsAboutToBeRemoved(QModelIndex,int,int)));
        connect(m_source, SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this,     SLOT(sourceRowsInserted(QModelIndex,int,int)));
    }

    emit sourceChanged();

    refilter();
}

void VisualLogFilter::componentComplete(){
    m_componentReady = true;
    refilter();
}

QVariant VisualLogFilter::data(const QModelIndex &index, int role) const{
    if ( index.row() >= m_entries.size() )
        return QVariant();

    if ( role == VisualLogBaseModel::Msg )
        return entryDataAt(index.row());
    else if ( role == VisualLogBaseModel::Prefix )
        return entryPrefixAt(index.row());

    return QVariant();
}

int VisualLogFilter::rowCount(const QModelIndex &) const{
    return m_entries.size();
}

QVariant VisualLogFilter::entryDataAt(int index) const{
    if ( m_source ){
        return m_source->entryDataAt(m_entries[index]);
    }

    return QVariant();
}

QString VisualLogFilter::entryPrefixAt(int index) const{
    if ( m_source ){
        return m_source->entryPrefixAt(m_entries[index]);
    }

    return QString();
}

const VisualLogEntry &VisualLogFilter::entryAt(int index) const{
    Q_ASSERT(m_source);

    return m_source->entryAt(m_entries[index]);
}

void VisualLogFilter::setIsIndexing(bool isIndexing){
    if ( m_isIndexing == isIndexing )
        return;

    m_isIndexing = isIndexing;
    emit isIndexingChanged();
}

void VisualLogFilter::refilter(){
    if ( !m_source || !m_componentReady )
        return;

    beginResetModel();
    m_entries.clear();
    endResetModel();

    int totalEntries = m_source->totalEntries(); // monitor all other entries through signals

    if ( totalEntries > 0 ){
        QFuture<QList<int> > future = QtConcurrent::run(
            [this](VisualLogBaseModel* source, int totalEntries){
                QList<int> entries;
                for ( int i = 0; i < totalEntries; ++i ){
                    const VisualLogEntry& entry = source->entryAt(i);
                    if ( filterEntry(entry) )
                        entries.append(i);
                }

                return entries;
            }, m_source, totalEntries
        );
        m_workerWatcher.setFuture(future);
    }
}

bool VisualLogFilter::filterEntry(const VisualLogEntry &entry){
    if ( !m_tag.isEmpty() ){
        if ( entry.tag.indexOf(m_tag) == -1 )
            return false;
    }
    if ( m_prefix.searchType() != SearchQuery::None ){
        if ( m_prefix.locateIn(entry.prefix ) == -1 )
            return false;
    }
    if ( m_search.searchType() != SearchQuery::None ){
        if ( m_search.locateIn(entry.data) == -1 )
            return false;
    }
    return true;
}

void VisualLogFilter::setPrefix(QJSValue prefix){
    SearchQuery sq(prefix, PluginContext::engine()->engine());
    if ( sq == m_prefix )
        return;

    m_prefix = sq;
    emit prefixChanged();

    refilter();
}

QJSValue VisualLogFilter::prefix() const{
    return m_prefix.toJs(PluginContext::engine()->engine());
}

// this slot gets called after the worker finishes, but also gets called only after
// control is returned to the event loop. Any changes in between will be lost, therefore
// we handle the changes separately, using the m_workerIgnoreResult variable.

void VisualLogFilter::refilterReady(){
    if ( m_workerIgnoreResult ){
        m_workerIgnoreResult = false;
        return;
    }
    beginResetModel();
    m_entries = m_workerWatcher.result();
    endResetModel();
    setIsIndexing(false);
}

void VisualLogFilter::sourceDestroyed(){
    setSource(0);
}

void VisualLogFilter::sourceModelReset(){
    refilter();
}

void VisualLogFilter::sourceModelAboutToReset(){
    m_workerIgnoreResult = true;
    m_workerWatcher.waitForFinished();
    beginResetModel();
    m_entries.clear();
    endResetModel();
}

void VisualLogFilter::sourceRowsAboutToBeRemoved(const QModelIndex &, int, int to){
    if ( m_workerWatcher.isRunning() ){
        m_workerWatcher.waitForFinished();
        m_workerIgnoreResult = true;
        beginResetModel();
        m_entries = m_workerWatcher.result();
        endResetModel();
    }

    if ( m_entries.empty() )
        return;
    if ( m_entries.first() > to )
        return;

    for ( int i = 0; i < m_entries.size(); ++i ){
        if ( m_entries[i] > to ){
            beginRemoveRows(QModelIndex(), 0, i - 1);
            m_entries.erase(m_entries.begin(), m_entries.begin() + i - 1);
            endRemoveRows();
            return;
        }
    }

    beginResetModel();
    m_entries.clear();
    endResetModel();
}

void VisualLogFilter::sourceRowsInserted(const QModelIndex &, int from, int to){
    if ( m_workerWatcher.isRunning() ){
        m_workerWatcher.waitForFinished();
        m_workerIgnoreResult = true;
        beginResetModel();
        m_entries = m_workerWatcher.result();
        endResetModel();
    }

    if ( from == to ){
        const VisualLogEntry& entry = m_source->entryAt(from);
        if ( filterEntry(entry) ){
            beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size());
            m_entries.append(from);
            endInsertRows();
        }
    } else {
        QList<int> entries;
        for ( int i = from; i < to + 1; ++i ){
            const VisualLogEntry& entry = m_source->entryAt(i);
            if ( filterEntry(entry) ){
                entries.append(i);
            }
        }

        if ( entries.size() > 0 ){
            beginInsertRows(QModelIndex(), m_entries.size(), m_entries.size() + entries.size());
            m_entries << entries;
            endInsertRows();
        }
    }
}

void VisualLogFilter::setSearch(QJSValue search){
    SearchQuery sq(search, PluginContext::engine()->engine());
    if ( sq == m_search )
        return;

    m_search = sq;
    emit searchChanged();

    refilter();
}

QJSValue VisualLogFilter::search() const{
    return m_search.toJs(PluginContext::engine()->engine());
}

}// namespace
