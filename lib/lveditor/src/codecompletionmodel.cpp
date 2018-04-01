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

#include "live/codecompletionmodel.h"

namespace lv{

/*!
  \class lv::CodeCompletionModel
  \inmodule lveditor_cpp
  \brief Code Completion Model used by the code completion assist

 */

CodeCompletionModel::CodeCompletionModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_completionContext(0)
    , m_completionPosition(0)
    , m_isEnabled(false)
{
    m_roles[CodeCompletionModel::Label]         = "label";
    m_roles[CodeCompletionModel::Info]          = "info";
    m_roles[CodeCompletionModel::Category]      = "category";
    m_roles[CodeCompletionModel::Completion]    = "completion";
    m_roles[CodeCompletionModel::Documentation] = "documentation";
}

CodeCompletionModel::~CodeCompletionModel(){
    delete m_completionContext;
}

QVariant CodeCompletionModel::data(const QModelIndex &index, int role) const{
    int suggestionIndex = m_filteredSuggestions[index.row()];
    if ( role == CodeCompletionModel::Label ){
        return m_suggestions[suggestionIndex].label();
    } else if ( role == CodeCompletionModel::Info ){
        return m_suggestions[suggestionIndex].info();
    } else if ( role == CodeCompletionModel::Category ){
        return m_suggestions[suggestionIndex].category();
    } else if ( role == CodeCompletionModel::Completion ){
        return m_suggestions[suggestionIndex].completion();
    } else if ( role == CodeCompletionModel::Documentation ){
        return m_suggestions[suggestionIndex].info();
    }
    return QVariant();
}

void CodeCompletionModel::resetModel(){
    beginResetModel();
    m_suggestions.clear();
    m_filteredSuggestions.clear();
    endResetModel();
}

void CodeCompletionModel::setSuggestions(
    const QList<CodeCompletionSuggestion> &suggestions,
    const QString &suggestionFilter)
{
    beginResetModel();
    m_suggestions = suggestions;
    m_filter = suggestionFilter;
    updateFilters();
    endResetModel();
}

void CodeCompletionModel::setFilter(const QString &filter){
    m_filter = filter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

void CodeCompletionModel::setCompletionContext(CodeCompletionContext *context){
    m_completionContext = context;
}

void CodeCompletionModel::removeCompletionContext(){
    delete m_completionContext;
    m_completionContext = 0;
}

void CodeCompletionModel::updateFilters(){
    m_filteredSuggestions.clear();
    for ( int i = 0; i < m_suggestions.size(); ++i ){
        if ( m_suggestions[i].label().startsWith(m_filter, Qt::CaseInsensitive) )
            m_filteredSuggestions.append(i);
    }
    if ( m_filteredSuggestions.isEmpty() )
        disable();
}

}// namespace
