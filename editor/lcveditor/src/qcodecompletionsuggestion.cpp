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

}// namespace
