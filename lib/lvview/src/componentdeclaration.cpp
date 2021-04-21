#include "componentdeclaration.h"

namespace lv{

ComponentDeclaration::ComponentDeclaration(const QString &id, const QUrl &url)
    : m_id(id)
    , m_url(url)
{
}

}// namespace
