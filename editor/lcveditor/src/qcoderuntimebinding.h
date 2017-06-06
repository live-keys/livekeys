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

#ifndef QCODERUNTIMEBINDING_H
#define QCODERUNTIMEBINDING_H

#include "qlcveditorglobal.h"
#include "qcodedeclaration.h"
#include "qcodeconverter.h"
#include <QObject>

namespace lcv{

class QProjectDocumentBlockData;
class Q_LCVEDITOR_EXPORT QCodeRuntimeBinding : public QObject{

    Q_OBJECT

    friend class QProjectDocumentBlockData;

public:
    QCodeRuntimeBinding(const QStringList &identifierChain, QProjectDocument* parent = 0);
    QCodeRuntimeBinding(QCodeDeclaration::Ptr declaration);
    ~QCodeRuntimeBinding();

    void setConverter(QCodeConverter* converter);

    QCodeDeclaration::Ptr declaration();
    QCodeDeclaration::ConstPtr declaration() const;

    int position() const;
    int valuePosition() const;
    int length() const;

    bool isModifiedByEngine() const;

    QProjectDocumentBlockData* parentBlock();

public slots:
    void updateValue();

public:
    QCodeDeclaration::Ptr      m_declaration;
    QProjectDocumentBlockData* m_parentBlock;
    QCodeConverter*            m_converter;
    bool                       m_modifiedByEngine;
};

inline void QCodeRuntimeBinding::setConverter(QCodeConverter *converter){
    m_converter = converter;
}

inline QCodeDeclaration::Ptr QCodeRuntimeBinding::declaration(){
    return m_declaration;
}

inline QCodeDeclaration::ConstPtr QCodeRuntimeBinding::declaration() const{
    return m_declaration;
}

inline int QCodeRuntimeBinding::valuePosition() const{
    return declaration()->valuePosition();
}

inline bool QCodeRuntimeBinding::isModifiedByEngine() const{
    return m_modifiedByEngine;
}

inline QProjectDocumentBlockData *QCodeRuntimeBinding::parentBlock(){
    return m_parentBlock;
}

}// namespace

#endif // QCODERUNTIMEBINDING_H
