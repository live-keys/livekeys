/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef LVQMLEDITFRAGMENT_H
#define LVQMLEDITFRAGMENT_H

#include "live/lveditqmljsglobal.h"
#include "live/qmldeclaration.h"

#include <QVariant>
#include <QQmlProperty>

namespace lv{

class CodePalette;
class CodeQmlHandler;
class QmlBindingPath;
class QmlBindingChannel;
class QmlBindingSpan;
class QmlBindingSpanModel;

class LV_EDITQMLJS_EXPORT QmlEditFragment : public QObject{

    Q_OBJECT
    Q_PROPERTY(QObject* visualParent READ visualParent WRITE setVisualParent NOTIFY visualParentChanged)

public:
    /** ProjectDocument section type for this QmlEditFragment */
    enum SectionType{
        /** Section Value */
        Section = 1001
    };

public:
    QmlEditFragment(QmlDeclaration::Ptr declaration, QObject* parent = nullptr);
    virtual ~QmlEditFragment();

    QmlBindingSpan* bindingSpan();

    void setPaletteForBinding(CodePalette* palette);
    void setRelativeBinding(const QSharedPointer<QmlBindingPath>& bp);

    bool hasPalette(CodePalette* palette);
    CodePalette* palette(const QString& type);
    void addPalette(CodePalette* palette);
    void removePalette(CodePalette* palette);
    QList<CodePalette*>::iterator begin();
    QList<CodePalette*>::iterator end();

    void removeBindingPalette();
    void setBindingPalette(CodePalette* palette);

    void addChildFragment(QmlEditFragment* edit);
    void removeChildFragment(QmlEditFragment* edit);
    QmlEditFragment* findChildFragment(QmlEditFragment* edit);
    const QList<QmlEditFragment*> childFragments();

    QmlDeclaration::Ptr declaration() const;

    void write(const QString& code);
    QString readValueText() const;

    void updatePaletteValue(CodePalette* palette);

    QObject* visualParent() const;
    void setVisualParent(QObject* visualParent);

    QmlEditFragment* rootFragment();

    void emitRemoval();

public slots:
    int position();
    int valuePosition() const;
    int valueLength() const;

    bool isForObject() const;
    bool isForProperty() const;

    int totalPalettes() const;
    lv::QmlEditFragment* parentFragment();
    lv::CodePalette* bindingPalette();
    lv::QmlBindingSpanModel *bindingModel(lv::CodeQmlHandler* codeHandler);

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
    QmlDeclaration::Ptr  m_declaration;

    QList<CodePalette*>  m_palettes;
    CodePalette*         m_bindingPalette;

    QList<QmlEditFragment*> m_childFragments;

    QmlBindingSpan*                m_bindingSpan;
    QSharedPointer<QmlBindingPath> m_relativeBinding;

    bool                 m_bindingUse;
    bool                 m_paletteUse;
    QObject*             m_visualParent;

    QmlBindingSpanModel*    m_bindingSpanModel;
};

/// \brief Returns the binding channel associated with this object.
inline QmlBindingSpan *QmlEditFragment::bindingSpan(){
    return m_bindingSpan;
}

inline CodePalette *QmlEditFragment::bindingPalette(){
    return m_bindingPalette;
}

inline bool QmlEditFragment::hasPalette(CodePalette *palette){
    for ( auto it = begin(); it != end(); ++it ){
        if ( *it == palette )
            return true;
    }
    return false;
}

inline QList<CodePalette *>::iterator QmlEditFragment::begin(){
    return m_palettes.begin();
}

inline QList<CodePalette *>::iterator QmlEditFragment::end(){
    return m_palettes.end();
}

inline const QList<QmlEditFragment *> QmlEditFragment::childFragments(){
    return m_childFragments;
}

/// \brief Returns the lv::CodeDeclaration associated with this object.
inline QmlDeclaration::Ptr QmlEditFragment::declaration() const{
    return m_declaration;
}

inline QObject *QmlEditFragment::visualParent() const{
    return m_visualParent;
}

inline int QmlEditFragment::position(){
    return m_declaration->position();

}

inline void QmlEditFragment::setVisualParent(QObject *visualParent){
    if (m_visualParent == visualParent)
        return;

    m_visualParent = visualParent;
    emit visualParentChanged();
}

}// namespace

#endif // LVDOCUMENTEDITFRAGMENT_H
