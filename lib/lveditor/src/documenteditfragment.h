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

#ifndef LVDOCUMENTEDITFRAGMENT_H
#define LVDOCUMENTEDITFRAGMENT_H

#include <QVariant>

#include "live/lveditorglobal.h"
#include "live/codedeclaration.h"

namespace lv{

class CodeConverter;
class CodeRuntimeBinding;
class LV_EDITOR_EXPORT DocumentEditFragment{

public:
    enum ActionType{
        Edit,
        Adjust
    };

    enum SectionType{
        Section = 1001
    };

public:
    DocumentEditFragment(CodeDeclaration::Ptr declaration, CodeConverter* converter = 0);
    virtual ~DocumentEditFragment();

    int valuePosition() const;
    int valueLength() const;
    CodeConverter* converter();

    void setActionType(ActionType action);
    ActionType actionType() const;

    virtual void commit(const QVariant&){}

    CodeDeclaration::Ptr declaration() const;

    void setRuntimeBinding(CodeRuntimeBinding* runtimeBinding);
    CodeRuntimeBinding* runtimeBinding();

private:
    CodeDeclaration::Ptr m_declaration;
    CodeConverter*       m_converter;
    CodeRuntimeBinding*  m_binding;

    ActionType m_actionType;
};

inline CodeConverter *DocumentEditFragment::converter(){
    return m_converter;
}

inline void DocumentEditFragment::setActionType(DocumentEditFragment::ActionType action){
    m_actionType = action;
}

inline void DocumentEditFragment::setRuntimeBinding(CodeRuntimeBinding *binding){
    m_binding = binding;
}

inline CodeRuntimeBinding *DocumentEditFragment::runtimeBinding(){
    return m_binding;
}

inline DocumentEditFragment::ActionType DocumentEditFragment::actionType() const{
    return m_actionType;
}

inline CodeDeclaration::Ptr DocumentEditFragment::declaration() const{
    return m_declaration;
}

}// namespace

#endif // LVDOCUMENTEDITFRAGMENT_H
