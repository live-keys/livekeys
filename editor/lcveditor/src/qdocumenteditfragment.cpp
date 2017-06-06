#include "qdocumenteditfragment.h"
#include "qcodedeclaration.h"

namespace lcv{

QDocumentEditFragment::QDocumentEditFragment(QCodeDeclaration::Ptr declaration, QCodeConverter *converter)
    : m_declaration(declaration)
    , m_converter(converter)
    , m_binding(0)
    , m_actionType(QDocumentEditFragment::Edit)
{

}

QDocumentEditFragment::~QDocumentEditFragment(){
}

int QDocumentEditFragment::valuePosition() const{
    return m_declaration->valuePosition();
}

int QDocumentEditFragment::valueLength() const{
    return m_declaration->valueLength();
}

}// namespace
