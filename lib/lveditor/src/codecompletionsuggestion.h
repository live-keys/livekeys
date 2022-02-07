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

#ifndef LVCODECOMPLETIONSUGGESTION_H
#define LVCODECOMPLETIONSUGGESTION_H

#include <QString>
#include "live/lveditorglobal.h"

namespace lv{

class LV_EDITOR_EXPORT CodeCompletionSuggestion{

public:
    CodeCompletionSuggestion(
        const QString& label,
        const QString& info,
        const QString& category,
        const QString& completion,
        const QString& documentation = "");
    ~CodeCompletionSuggestion();

    const QString& label() const;
    const QString& info() const;
    const QString& category() const;
    const QString& completion() const;
    const QString& documentation() const;

    static bool compare(const CodeCompletionSuggestion& first, const CodeCompletionSuggestion& second);

private:
    QString m_label;
    QString m_info;
    QString m_category;
    QString m_completion;
    QString m_documentation;
};

/**
 * \brief Label getter
 */
inline const QString &CodeCompletionSuggestion::label() const{
    return m_label;
}

/**
 * \brief Completion getter
 */
inline const QString &CodeCompletionSuggestion::completion() const{
    return m_completion;
}

/**
 * \brief Documentation getter
 */
inline const QString &CodeCompletionSuggestion::documentation() const{
    return m_documentation;
}

/**
 * \brief Info getter
 */
inline const QString &CodeCompletionSuggestion::info() const{
    return m_info;
}

/**
 * \brief Category getter
 */
inline const QString &CodeCompletionSuggestion::category() const{
    return m_category;
}

}// namespace

#endif // LVCODECOMPLETIONSUGGESTION_H
