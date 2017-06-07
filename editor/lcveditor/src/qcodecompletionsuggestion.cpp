/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#include "qcodecompletionsuggestion.h"

namespace lcv{

/*!
  \class lcv::QCodeCompletionSuggestion
  \inmodule lcveditor_cpp
  \brief Code completion suggestion used in the QCodeCompletionModel.
 */

QCodeCompletionSuggestion::QCodeCompletionSuggestion(
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

QCodeCompletionSuggestion::~QCodeCompletionSuggestion(){

}

bool QCodeCompletionSuggestion::compare(
        const QCodeCompletionSuggestion &first,
        const QCodeCompletionSuggestion &second)
{
    if ( first.category() == second.category() )
        return first.label() < second.label();
    return first.category() < second.category();
}

}// namespace
