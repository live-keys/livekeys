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

#include "live/codecompletionsuggestion.h"

namespace lv{

/**
  \class lv::CodeCompletionSuggestion
  \ingroup lveditor
  \brief Code completion suggestion used in the QCodeCompletionModel.
 */

/**
 * \brief Default constructor for CodeCompletionSuggestion, simply setting the given parameters
 */
CodeCompletionSuggestion::CodeCompletionSuggestion(
        const QString &label,
        const QString &info,
        const QString &category,
        const QString &completion)
    : m_label(label)
    , m_info(info)
    , m_category(category)
    , m_completion(completion)
{
}

/**
 * \brief Blank destructor of CodeCOmpletionSuggestion
 */
CodeCompletionSuggestion::~CodeCompletionSuggestion(){

}

/**
 * \brief Comparator function to be used for sorting
 */
bool CodeCompletionSuggestion::compare(
        const CodeCompletionSuggestion &first,
        const CodeCompletionSuggestion &second)
{
    if ( first.category() == second.category() )
        return first.label() < second.label();
    return first.category() < second.category();
}

}// namespace
