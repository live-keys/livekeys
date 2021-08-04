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
#include "live/documentqmlinfo.h"

#include <QVariant>
#include <QJSValue>
#include <QQmlProperty>

#include <tuple>

namespace lv{

class DocumentQmlChannels;
class CodePalette;
class LanguageQmlHandler;
class QmlBindingPath;
class QmlBindingChannel;
class CodeCompletionModel;

class LV_EDITQMLJS_EXPORT QmlEditFragment : public QObject{

    Q_OBJECT
    Q_PROPERTY(QObject*             visualParent  READ visualParent  WRITE setVisualParent NOTIFY visualParentChanged)
    Q_PROPERTY(int                  refCount      READ refCount      NOTIFY refCountChanged)
    Q_PROPERTY(Location             location      READ location      CONSTANT)
    Q_PROPERTY(Location             valueLocation READ valueLocation CONSTANT)
    Q_PROPERTY(bool                 isWritable    READ isWritable    CONSTANT)
    Q_PROPERTY(QJSValue             whenBinding   READ whenBinding   WRITE setWhenBinding NOTIFY whenBindingChanged)
    Q_PROPERTY(lv::LanguageQmlHandler*  codeHandler   READ codeHandler   CONSTANT)
    Q_PROPERTY(bool                 isNull        READ isNull        NOTIFY isNullChanged)
    Q_ENUMS(FragmentType)

public:
    /** ProjectDocument section type for this QmlEditFragment */
    enum SectionType{
        /** Section Value */
        Section = 1001
    };

    enum Location {
        Imports,
        Object,
        Component,
        Property,
        Slot
    };
    Q_ENUM(Location)

    enum FragmentType {
        Builder = 1,
        ReadOnly = 2,
        Group = 4,
        GroupChild = 8
    };

public:
    QmlEditFragment(QmlDeclaration::Ptr declaration, lv::LanguageQmlHandler* codeHandler, QObject* parent = nullptr);
    virtual ~QmlEditFragment();

    static QObject* createObject(
        const DocumentQmlInfo::ConstPtr& info,
        const QString& declaration,
        const QString& path,
        QObject* parent = nullptr,
        QQmlContext* context = nullptr,
        QList<std::tuple<QString, QString, QString> > props = QList<std::tuple<QString, QString, QString> >()
    );

    void setPaletteForBinding(CodePalette* palette);

    bool hasPalette(CodePalette* palette);
    CodePalette* palette(const QString& type);
    void addPalette(CodePalette* palette);
    void removePalette(CodePalette* palette);
    QList<CodePalette*>::iterator begin();
    QList<CodePalette*>::iterator end();

    void removeBindingPalette();
    void setBindingPalette(CodePalette* palette);

    void addChildFragment(QmlEditFragment* edit);
    QmlEditFragment* findChildFragment(QmlEditFragment* edit);

    QmlDeclaration::Ptr declaration() const;

    void updatePaletteValue(CodePalette* palette);
    void initializePaletteValue(CodePalette* palette);

    QObject* visualParent() const;
    void setVisualParent(QObject* visualParent);

    QmlEditFragment* rootFragment();

    void setObjectInitializeType(const QmlTypeReference& type);
    const QmlTypeReference& objectInitializeType() const;

    void emitRemoval();

    QJSValue& whenBinding();
    void setWhenBinding(const QJSValue& whenBinding);

    void setChannel(QSharedPointer<QmlBindingChannel> channel);
    const QSharedPointer<QmlBindingChannel> channel() const;

    QSharedPointer<QmlBindingPath> fullBindingPath();

    Location location() const;
    Location valueLocation() const;
    bool isWritable() const;
    lv::LanguageQmlHandler* codeHandler() const;

    void setObjectId(QString id);
    const QList<lv::QmlEditFragment*>& childFragments();
    void checkIfGroup();

public slots:
    //TOMOVE
    int fragmentType() const;
    bool isOfFragmentType(FragmentType type) const;
    void addFragmentType(FragmentType type);
    void removeFragmentType(FragmentType type);
    bool isGroup() const;

    void signalChildAdded(lv::QmlEditFragment* ef, const QJSValue& context = QJSValue());
    void suggestionsForExpression(const QString& expression, lv::CodeCompletionModel* model, bool suggestFunctions);
    // ----------------------

    int position();
    int valuePosition() const;
    int valueLength() const;
    int length() const;

    QString type() const;
    QString typeName() const;
    QString identifier() const;
    QString objectInitializerType() const;
    QString objectId();

    bool isBuilder() const;
    void rebuild();
    QStringList bindingPath();

    bool isNull();
    bool isMethod();

    void commit(const QVariant& value);
    QObject* readObject();
    QObject* propertyObject();

    QString defaultValue() const;
    QString readValueText() const;
    QJSValue readValueConnection() const;
    QVariant parse();

    bool bindExpression(const QString& expression);
    bool bindFunctionExpression(const QString& expression);

    void writeProperties(const QJSValue& properties);
    void write(const QJSValue options);
    void writeCode(const QString& code);

    int totalPalettes() const;
    QJSValue paletteList() const;
    lv::CodePalette* bindingPalette();
    void updateBindings();

    lv::QmlEditFragment* parentFragment();
    QJSValue getChildFragments() const;
    void removeChildFragment(QmlEditFragment* edit);

    void incrementRefCount();
    void decrementRefCount();
    int refCount();


    void __updateFromPalette();
    void __updateValue();
    void __channelObjectErased();
    void __inputRunnableObjectReady();
    void __selectedChannelChanged();

signals:
    void visualParentChanged();
    void connectionChanged(int index);
    void aboutToRemovePalette(lv::CodePalette* palette);
    void aboutToBeRemoved();
    void paletteListEmpty();

    void objectAdded(lv::QmlEditFragment* obj);
    void childAdded(lv::QmlEditFragment* ef, QJSValue context);

    void refCountChanged();
    void whenBindingChanged();
    void typeChanged();
    void isNullChanged();

private:
    static QString buildCode(const QJSValue& value, bool additionalBraces = false);
    void checkIsNull();

    QmlDeclaration::Ptr  m_declaration;

    QList<CodePalette*>  m_palettes;
    CodePalette*         m_bindingPalette;

    QList<QmlEditFragment*> m_childFragments;

    QSharedPointer<QmlBindingChannel> m_channel;

    bool                    m_bindingUse;
    bool                    m_paletteUse;
    QObject*                m_visualParent;

    QmlTypeReference        m_objectInitializeType;
    int                     m_refCount;
    QString                 m_objectId;
    Location                m_valueLocation;
    QJSValue                m_whenBinding;
    int                     m_fragmentType;
    lv::LanguageQmlHandler*     m_codeHandler;
};

inline CodePalette *QmlEditFragment::bindingPalette(){
    return m_bindingPalette;
}

inline lv::LanguageQmlHandler *QmlEditFragment::codeHandler() const {
    return m_codeHandler;
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

inline const QList<QmlEditFragment *>& QmlEditFragment::childFragments(){
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

inline QJSValue& QmlEditFragment::whenBinding(){
    return m_whenBinding;
}

inline void QmlEditFragment::setWhenBinding(const QJSValue& whenBinding){
    m_whenBinding = whenBinding;
    emit whenBindingChanged();
}

inline const QSharedPointer<QmlBindingChannel> QmlEditFragment::channel() const{
    return m_channel;
}

}// namespace

#endif // LVDOCUMENTEDITFRAGMENT_H
