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
