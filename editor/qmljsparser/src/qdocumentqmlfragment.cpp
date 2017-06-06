#include "qdocumentqmlfragment.h"

namespace lcv{

QDocumentQmlFragment::QDocumentQmlFragment(QCodeDeclaration::Ptr declaration, QCodeConverter *palette, const QQmlProperty &property)
    : QDocumentEditFragment(declaration, palette)
    , m_property(property)
{
}

QDocumentQmlFragment::~QDocumentQmlFragment(){
}

}// namespace
