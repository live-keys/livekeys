#include "qdocumentcodestate.h"
#include "qdocumenteditfragment.h"

namespace lcv{

QDocumentCodeState::QDocumentCodeState()
    : m_editingFragment(0)
{
}

QDocumentCodeState::~QDocumentCodeState(){
    delete m_editingFragment;
}

void QDocumentCodeState::setEditingFragment(QDocumentEditFragment *fragment){
    clearEditingFragment();
    m_editingFragment = fragment;
}

void QDocumentCodeState::clearEditingFragment(){
    if ( m_editingFragment != 0 ){
        delete m_editingFragment;
        m_editingFragment = 0;
    }
}

}// namespace
