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

#ifndef LVCODERUNTIMEBINDING_H
#define LVCODERUNTIMEBINDING_H

#include <QObject>

#include "live/lveditorglobal.h"
#include "live/codedeclaration.h"
#include "live/codeconverter.h"

namespace lv{

class ProjectDocumentBlockData;
class LV_EDITOR_EXPORT CodeRuntimeBinding : public QObject{

    Q_OBJECT

    friend class ProjectDocumentBlockData;

public:
    CodeRuntimeBinding(const QStringList &identifierChain, ProjectDocument* parent = 0);
    CodeRuntimeBinding(CodeDeclaration::Ptr declaration);
    ~CodeRuntimeBinding();

    void setConverter(CodeConverter* converter);

    CodeDeclaration::Ptr declaration();
    CodeDeclaration::ConstPtr declaration() const;

    int position() const;
    int valuePosition() const;
    int length() const;

    bool isModifiedByEngine() const;

    ProjectDocumentBlockData* parentBlock();

public slots:
    void updateValue();

public:
    CodeDeclaration::Ptr      m_declaration;
    ProjectDocumentBlockData* m_parentBlock;
    CodeConverter*            m_converter;
    bool                       m_modifiedByEngine;
};

inline void CodeRuntimeBinding::setConverter(CodeConverter *converter){
    m_converter = converter;
}

inline CodeDeclaration::Ptr CodeRuntimeBinding::declaration(){
    return m_declaration;
}

inline CodeDeclaration::ConstPtr CodeRuntimeBinding::declaration() const{
    return m_declaration;
}

inline int CodeRuntimeBinding::valuePosition() const{
    return declaration()->valuePosition();
}

inline bool CodeRuntimeBinding::isModifiedByEngine() const{
    return m_modifiedByEngine;
}

inline ProjectDocumentBlockData *CodeRuntimeBinding::parentBlock(){
    return m_parentBlock;
}

}// namespace

#endif // LVCODERUNTIMEBINDING_H
