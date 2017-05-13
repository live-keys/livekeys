#include "qdocumenteditfragment.h"

namespace lcv{

QDocumentEditFragment::QDocumentEditFragment(int position, int length, QCodeConverter *converter)
    : m_position(position)
    , m_length(length)
    , m_converter(converter)
    , m_actionType(QDocumentEditFragment::Edit)
{

}

QDocumentEditFragment::~QDocumentEditFragment(){
}

}// namespace
