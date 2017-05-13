#ifndef QDOCUMENTCODESTATE_H
#define QDOCUMENTCODESTATE_H

#include "qlcveditorglobal.h"

namespace lcv{

class QDocumentEditFragment;
class Q_LCVEDITOR_EXPORT QDocumentCodeState{

public:
    QDocumentCodeState();
    ~QDocumentCodeState();

    void setEditingFragment(QDocumentEditFragment* fragment);
    QDocumentEditFragment* editingFragment();
    void clearEditingFragment();

private:
    QDocumentEditFragment* m_editingFragment;

};

inline QDocumentEditFragment *QDocumentCodeState::editingFragment(){
    return m_editingFragment;
}

}// namespace

#endif // QDOCUMENTCODESTATE_H
