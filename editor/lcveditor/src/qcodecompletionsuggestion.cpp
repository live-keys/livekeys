#include "qcodecompletionsuggestion.h"

namespace lcv{

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
