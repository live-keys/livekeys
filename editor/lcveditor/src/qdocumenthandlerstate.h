#ifndef QDOCUMENTHANDLERSTATE_H
#define QDOCUMENTHANDLERSTATE_H

#include "qlcveditorglobal.h"

namespace lcv{

class QDocumentEditFragment;
class Q_LCVEDITOR_EXPORT QDocumentHandlerState{

public:
    QDocumentHandlerState();
    ~QDocumentHandlerState();

    void setEditingFragment(QDocumentEditFragment* fragment);
    QDocumentEditFragment* editingFragment();
    void clearEditingFragment();

private:
    QDocumentEditFragment* m_editingFragment;

};

inline QDocumentEditFragment *QDocumentHandlerState::editingFragment(){
    return m_editingFragment;
}

}// namespace

#endif // QDOCUMENTHANDLERSTATE_H
