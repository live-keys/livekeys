/****************************************************************************
**
** Copyright (C) 2014-2018 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of Live CV Application.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
****************************************************************************/

#ifndef LVDOCUMENTHANDLERSTATE_H
#define LVDOCUMENTHANDLERSTATE_H

#include "live/lveditorglobal.h"

namespace lv{

class DocumentEditFragment;
class LV_EDITOR_EXPORT DocumentHandlerState{

public:
    DocumentHandlerState();
    ~DocumentHandlerState();

    void setEditingFragment(DocumentEditFragment* fragment);
    DocumentEditFragment* editingFragment();
    void clearEditingFragment();

private:
    DocumentEditFragment* m_editingFragment;

};

inline DocumentEditFragment *DocumentHandlerState::editingFragment(){
    return m_editingFragment;
}

}// namespace

#endif // LVDOCUMENTHANDLERSTATE_H
