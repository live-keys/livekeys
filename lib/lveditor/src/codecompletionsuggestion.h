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
        const QString& completion);
    ~CodeCompletionSuggestion();

    const QString& label() const;
    const QString& info() const;
    const QString& category() const;
    const QString& completion() const;

    static bool compare(const CodeCompletionSuggestion& first, const CodeCompletionSuggestion& second);

private:
    QString m_label;
    QString m_info;
    QString m_category;
    QString m_completion;
};

inline const QString &CodeCompletionSuggestion::label() const{
    return m_label;
}

inline const QString &CodeCompletionSuggestion::completion() const{
    return m_completion;
}

inline const QString &CodeCompletionSuggestion::info() const{
    return m_info;
}

inline const QString &CodeCompletionSuggestion::category() const{
    return m_category;
}

}// namespace

#endif // LVCODECOMPLETIONSUGGESTION_H
