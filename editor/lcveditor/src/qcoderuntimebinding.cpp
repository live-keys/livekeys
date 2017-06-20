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

#include "qcoderuntimebinding.h"
#include "qprojectdocument.h"
#include "qcodeconverter.h"

#include <QDebug>

namespace lcv{

QCodeRuntimeBinding::QCodeRuntimeBinding(const QStringList& identifierChain, QProjectDocument *parent)
    : QObject(parent)
    , m_declaration(QCodeDeclaration::create(identifierChain, parent))
    , m_parentBlock(0)
    , m_converter(0)
    , m_modifiedByEngine(false)
{
}

QCodeRuntimeBinding::QCodeRuntimeBinding(QCodeDeclaration::Ptr declaration)
    : QObject(declaration->document())
    , m_declaration(declaration)
    , m_parentBlock(0)
    , m_converter(0)
    , m_modifiedByEngine(0)
{
}

QCodeRuntimeBinding::~QCodeRuntimeBinding(){
    if( m_parentBlock )
        m_parentBlock->removeBinding(this);
}

void QCodeRuntimeBinding::updateValue(){
    if ( m_converter ){
        QString code = m_converter->serialize()->toCode(
            sender()->property(declaration()->identifierChain().last().toUtf8()), 0
        );
        if ( !code.isEmpty() ){
            declaration()->document()->updateBindingValue(this, code);
            m_modifiedByEngine = true;
        }
    }
}

int QCodeRuntimeBinding::position() const{
    return m_declaration->identifierPosition();
}

int QCodeRuntimeBinding::length() const{
    return m_declaration->length();
}

}// namespace
