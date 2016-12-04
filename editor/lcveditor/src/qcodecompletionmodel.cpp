#include "qcodecompletionmodel.h"

#include <QDebug>

namespace lcv{

QCodeCompletionModel::QCodeCompletionModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_completionPosition(0)
    , m_isEnabled(false)
    , m_completionContext(0)
{
    m_roles[QCodeCompletionModel::Label]         = "label";
    m_roles[QCodeCompletionModel::Info]          = "info";
    m_roles[QCodeCompletionModel::Category]      = "category";
    m_roles[QCodeCompletionModel::Completion]    = "completion";
    m_roles[QCodeCompletionModel::Documentation] = "documentation";
}

QCodeCompletionModel::~QCodeCompletionModel(){
    delete m_completionContext;
}

QVariant QCodeCompletionModel::data(const QModelIndex &index, int role) const{
    int suggestionIndex = m_filteredSuggestions[index.row()];
    if ( role == QCodeCompletionModel::Label ){
        return m_suggestions[suggestionIndex].label();
    } else if ( role == QCodeCompletionModel::Info ){
        return m_suggestions[suggestionIndex].info();
    } else if ( role == QCodeCompletionModel::Category ){
        return m_suggestions[suggestionIndex].category();
    } else if ( role == QCodeCompletionModel::Completion ){
        return m_suggestions[suggestionIndex].completion();
    } else if ( role == QCodeCompletionModel::Documentation ){
        return m_suggestions[suggestionIndex].info();
    }
    return QVariant();
}

void QCodeCompletionModel::resetModel(){
    beginResetModel();
    m_suggestions.clear();
    m_filteredSuggestions.clear();
    endResetModel();
}

void QCodeCompletionModel::setSuggestions(
    const QList<QCodeCompletionSuggestion> &suggestions,
    const QString &suggestionFilter)
{
    beginResetModel();
    m_suggestions = suggestions;
    m_filter = suggestionFilter;
    updateFilters();
    endResetModel();
}

void QCodeCompletionModel::setFilter(const QString &filter){
    m_filter = filter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void QCodeCompletionModel::setCompletionContext(QCodeCompletionContext *context){
    m_completionContext = context;
}

void QCodeCompletionModel::removeCompletionContext(){
    delete m_completionContext;
    m_completionContext = 0;
}

void QCodeCompletionModel::updateFilters(){
    m_filteredSuggestions.clear();
    for ( int i = 0; i < m_suggestions.size(); ++i ){
        if ( m_suggestions[i].label().startsWith(m_filter, Qt::CaseInsensitive) )
            m_filteredSuggestions.append(i);
    }
}

}// namespace
