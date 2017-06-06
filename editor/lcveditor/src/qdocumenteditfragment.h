/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef QDOCUMENTEDITFRAGMENT_H
#define QDOCUMENTEDITFRAGMENT_H

#include "qlcveditorglobal.h"
#include "qcodedeclaration.h"
#include <QVariant>

namespace lcv{

class QCodeConverter;
class QCodeRuntimeBinding;
class Q_LCVEDITOR_EXPORT QDocumentEditFragment{

public:
    enum ActionType{
        Edit,
        Adjust
    };

public:
    QDocumentEditFragment(QCodeDeclaration::Ptr declaration, QCodeConverter* converter = 0);
    virtual ~QDocumentEditFragment();

    int valuePosition() const;
    int valueLength() const;
    QCodeConverter* converter();

    void setActionType(ActionType action);
    ActionType actionType() const;

    virtual void commit(const QVariant&){}

    QCodeDeclaration::Ptr declaration();

    void setRuntimeBinding(QCodeRuntimeBinding* runtimeBinding);
    QCodeRuntimeBinding* runtimeBinding();

private:
    QCodeDeclaration::Ptr m_declaration;
    QCodeConverter*       m_converter;
    QCodeRuntimeBinding*  m_binding;

    ActionType m_actionType;
};

inline QCodeConverter *QDocumentEditFragment::converter(){
    return m_converter;
}

inline void QDocumentEditFragment::setActionType(QDocumentEditFragment::ActionType action){
    m_actionType = action;
}

inline void QDocumentEditFragment::setRuntimeBinding(QCodeRuntimeBinding *binding){
    m_binding = binding;
}

inline QCodeRuntimeBinding *QDocumentEditFragment::runtimeBinding(){
    return m_binding;
}

inline QDocumentEditFragment::ActionType QDocumentEditFragment::actionType() const{
    return m_actionType;
}

inline QCodeDeclaration::Ptr QDocumentEditFragment::declaration(){
    return m_declaration;
}

}// namespace

#endif // QDOCUMENTEDITFRAGMENT_H
