/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#include "live/codecompletionmodel.h"

namespace lv{

// CodeCompletionContext
// ----------------------------------------------------------------------------

CodeCompletionContext::~CodeCompletionContext(){
}


// CodeCompletionModel
// ----------------------------------------------------------------------------

/**
 * \class lv::CodeCompletionContext
 * \ingroup lveditor
 * \brief Base class for a code completion context, to be stored in a CodeCompletionModel
 */

/**
 * \class lv::CodeCompletionModel
 * \ingroup lveditor
 * \brief Code Completion Model used by the code completion assist
 */

/**
 * \brief Default constructor of CodeCompletionModel
*/
CodeCompletionModel::CodeCompletionModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_completionContext(nullptr)
    , m_completionPosition(0)
    , m_isEnabled(false)
{
    m_roles[CodeCompletionModel::Label]         = "label";
    m_roles[CodeCompletionModel::Info]          = "info";
    m_roles[CodeCompletionModel::Category]      = "category";
    m_roles[CodeCompletionModel::Completion]    = "completion";
    m_roles[CodeCompletionModel::Documentation] = "documentation";
}

/**
 * \brief Default destructor for CodeCompletionModel
 */
CodeCompletionModel::~CodeCompletionModel(){
    delete m_completionContext;
}

/**
 * \brief Override of standard model function for fetching data
*/
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
        return m_suggestions[suggestionIndex].documentation();
    }
    return QVariant();
}

/**
* \brief Function to reset the internal model representation
*/
void CodeCompletionModel::resetModel(){
    beginResetModel();
    m_suggestions.clear();
    m_filteredSuggestions.clear();
    endResetModel();
}

/**
 * \brief Simple function to assign code completion suggestions and filter
 */
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

/**
 * \brief Simple function to set the given filter
 */
void CodeCompletionModel::setFilter(const QString &filter){
    m_filter = filter;
    beginResetModel();
    updateFilters();
    endResetModel();
}

/**
 * \brief Sets given code completion context
 */
void CodeCompletionModel::setCompletionContext(CodeCompletionContext *context){
    m_completionContext = context;
}

/**
 * \brief Deletes the internal completion context
 */
void CodeCompletionModel::removeCompletionContext(){
    delete m_completionContext;
    m_completionContext = nullptr;
}

/**
 * \brief Filters suggestions according to the stored filter string
 */
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
