#include "qdocumentqmlfragment.h"

namespace lcv{

QDocumentQmlFragment::QDocumentQmlFragment(int position, int length, QCodeConverter *palette, const QQmlProperty &property)
    : QDocumentEditFragment(position, length, palette)
    , m_property(property)
{
}

QDocumentQmlFragment::~QDocumentQmlFragment(){
}

}// namespace
