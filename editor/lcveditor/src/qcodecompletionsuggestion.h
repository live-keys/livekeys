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

#ifndef QCODECOMPLETIONSUGGESTION_H
#define QCODECOMPLETIONSUGGESTION_H

#include "qlcveditorglobal.h"
#include <QString>

namespace lcv{

class Q_LCVEDITOR_EXPORT QCodeCompletionSuggestion{

public:
    QCodeCompletionSuggestion(
        const QString& label,
        const QString& info,
        const QString& category,
        const QString& completion);
    ~QCodeCompletionSuggestion();

    const QString& label() const;
    const QString& info() const;
    const QString& category() const;
    const QString& completion() const;

    static bool compare(const QCodeCompletionSuggestion& first, const QCodeCompletionSuggestion& second);

private:
    QString m_label;
    QString m_info;
    QString m_category;
    QString m_completion;
};

inline const QString &QCodeCompletionSuggestion::label() const{
    return m_label;
}

inline const QString &QCodeCompletionSuggestion::completion() const{
    return m_completion;
}

inline const QString &QCodeCompletionSuggestion::info() const{
    return m_info;
}

inline const QString &QCodeCompletionSuggestion::category() const{
    return m_category;
}

}// namespace

#endif // QCODECOMPLETIONSUGGESTION_H
