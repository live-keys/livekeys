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

#ifndef LVQMLEDITFRAGMENT_H
#define LVQMLEDITFRAGMENT_H

#include "live/lveditqmljsglobal.h"
#include "live/qmldeclaration.h"

#include <QVariant>
#include <QQmlProperty>

namespace lv{

class CodePalette;
class BindingPath;
class BindingChannel;

class LV_EDITQMLJS_EXPORT QmlEditFragment{

public:
    enum SectionType{
        Section = 1001
    };

public:
    QmlEditFragment(QmlDeclaration::Ptr declaration, CodePalette* palette = 0);
    virtual ~QmlEditFragment();

    int valuePosition() const;
    int valueLength() const;
    CodePalette* palette();

    void setExpressionPath(BindingPath* path);
    BindingPath* expressionPath();
    BindingChannel* bindingChannel();

    void setPaletteUse(bool paletteUse){ m_paletteUse = paletteUse; }
    bool paletteUse() const{ return m_paletteUse; }

    void setBindingUse(bool bindingUse){ m_bindingUse = bindingUse; }
    bool bindingUse() const{ return m_bindingUse; }

    QmlDeclaration::Ptr declaration() const;

    void write(const QString& code);
    QString readValueText() const;

private:
    QmlDeclaration::Ptr  m_declaration;
    CodePalette*         m_palette;
    BindingChannel*      m_bindingChannel;

    bool                 m_bindingUse;
    bool                 m_paletteUse;
};

inline CodePalette *QmlEditFragment::palette(){
    return m_palette;
}

inline BindingChannel *QmlEditFragment::bindingChannel(){
    return m_bindingChannel;
}

inline QmlDeclaration::Ptr QmlEditFragment::declaration() const{
    return m_declaration;
}

}// namespace

#endif // LVDOCUMENTEDITFRAGMENT_H
