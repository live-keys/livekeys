/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
**
** This file is part of Livekeys Application.
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

#ifndef LVEDITFRAGMENT_H
#define LVEDITFRAGMENT_H

#include "declaration.h"
#include <QVariant>
#include <QQmlProperty>
#include <memory>

namespace lv{

class CodePalette;
class LanguageLvHandler;
namespace el {

    class BindingPath;
}
class BindingChannel;
class BindingSpan;
class BindingSpanModel;

class LvEditFragment : public QObject{

    Q_OBJECT
    Q_PROPERTY(QObject* visualParent READ visualParent WRITE setVisualParent NOTIFY visualParentChanged)

public:
    /** ProjectDocument section type for this QmlEditFragment */
    enum SectionType{
        /** Section Value */
        Section = 1001
    };

public:
    LvEditFragment(el::Declaration::Ptr declaration, QObject* parent = nullptr);
    virtual ~LvEditFragment();

    BindingSpan* bindingSpan();

    void setPaletteForBinding(CodePalette* palette);
    void setRelativeBinding(const std::shared_ptr<el::BindingPath>& bp);

    bool hasPalette(CodePalette* palette);
    CodePalette* palette(const QString& type);
    void addPalette(CodePalette* palette);
    void removePalette(CodePalette* palette);
    QList<CodePalette*>::iterator begin();
    QList<CodePalette*>::iterator end();

    void removeBindingPalette();
    void setBindingPalette(CodePalette* palette);

    void addChildFragment(LvEditFragment* edit);
    void removeChildFragment(LvEditFragment* edit);
    LvEditFragment* findChildFragment(LvEditFragment* edit);
    const QList<LvEditFragment*> childFragments();

    el::Declaration::Ptr declaration() const;

    void write(const QString& code);
    QString readValueText() const;

    void updatePaletteValue(CodePalette* palette);

    QObject* visualParent() const;
    void setVisualParent(QObject* visualParent);

    LvEditFragment* rootFragment();

    void emitRemoval();

public slots:
    int position();
    int valuePosition() const;
    int valueLength() const;

    bool isForObject() const;
    bool isForProperty() const;

    int totalPalettes() const;
    lv::LvEditFragment* parentFragment();
    lv::CodePalette* bindingPalette();
    lv::BindingSpanModel *bindingModel(lv::LanguageLvHandler* codeHandler);

    QString type() const;

    QList<QObject*> getChildFragments() const;

    void updateValue();

    void __inputRunnableObjectReady();

signals:
    void visualParentChanged();
    void aboutToRemovePalette(lv::CodePalette* palette);
    void aboutToBeRemoved();
    void paletteListEmpty();

private:
    el::Declaration::Ptr                m_declaration;

    QList<CodePalette*>                 m_palettes;
    CodePalette*                        m_bindingPalette;

    QList<LvEditFragment*>              m_childFragments;

    BindingSpan*                        m_bindingSpan;
    std::shared_ptr<el::BindingPath>    m_relativeBinding;

    bool                                m_bindingUse;
    bool                                m_paletteUse;
    QObject*                            m_visualParent;

    BindingSpanModel*                   m_bindingSpanModel;
};

/// \brief Returns the binding channel associated with this object.
inline BindingSpan *LvEditFragment::bindingSpan(){
    return m_bindingSpan;
}

inline CodePalette *LvEditFragment::bindingPalette(){
    return m_bindingPalette;
}

inline bool LvEditFragment::hasPalette(CodePalette *palette){
    for ( auto it = begin(); it != end(); ++it ){
        if ( *it == palette )
            return true;
    }
    return false;
}

inline QList<CodePalette *>::iterator LvEditFragment::begin(){
    return m_palettes.begin();
}

inline QList<CodePalette *>::iterator LvEditFragment::end(){
    return m_palettes.end();
}

inline const QList<LvEditFragment *> LvEditFragment::childFragments(){
    return m_childFragments;
}

/// \brief Returns the lv::CodeDeclaration associated with this object.
inline el::Declaration::Ptr LvEditFragment::declaration() const{
    return m_declaration;
}

inline QObject *LvEditFragment::visualParent() const{
    return m_visualParent;
}

inline int LvEditFragment::position(){
    return m_declaration->position();

}

inline void LvEditFragment::setVisualParent(QObject *visualParent){
    if (m_visualParent == visualParent)
        return;

    m_visualParent = visualParent;
    emit visualParentChanged();
}

}// namespace

#endif // LVDOCUMENTEDITFRAGMENT_H
