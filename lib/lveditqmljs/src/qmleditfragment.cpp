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

#include "live/qmleditfragment.h"
#include "live/qmldeclaration.h"
#include "live/codepalette.h"
#include "bindingchannel.h"

namespace lv{

QmlEditFragment::QmlEditFragment(QmlDeclaration::Ptr declaration, lv::CodePalette *palette)
    : m_declaration(declaration)
    , m_palette(palette)
    , m_bindingChannel(new BindingChannel)
    , m_bindingUse(false)
    , m_paletteUse(false)
{
}

QmlEditFragment::~QmlEditFragment(){
    delete m_bindingChannel;
    m_palette->deleteLater();
}

int QmlEditFragment::valuePosition() const{
    return m_declaration->valuePosition();
}

int QmlEditFragment::valueLength() const{
    return m_declaration->valueLength();
}

void QmlEditFragment::setExpressionPath(BindingPath *path){
    m_bindingChannel->setExpressionPath(path);
}

BindingPath *QmlEditFragment::expressionPath(){
    return m_bindingChannel->expressionPath();
}

void QmlEditFragment::write(const QString &code){
    ProjectDocument* document = m_declaration->document();

    if ( document->editingStateIs(ProjectDocument::Palette))
        return;

    document->addEditingState(ProjectDocument::Palette);

    {
        QTextCursor tc(document->textDocument());
        tc.setPosition(valuePosition());
        tc.setPosition(valuePosition() + valueLength(), QTextCursor::KeepAnchor);
        tc.beginEditBlock();
        tc.removeSelectedText();
        tc.insertText(code);
        tc.endEditBlock();
    }

    document->removeEditingState(ProjectDocument::Palette);
}

QString QmlEditFragment::readValueText() const{
    QTextCursor tc(m_declaration->document()->textDocument());
    tc.setPosition(valuePosition());
    tc.setPosition(valuePosition() + valueLength(), QTextCursor::KeepAnchor);
    return tc.selectedText();
}

}// namespace
