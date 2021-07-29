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
class CodeQmlHandler;
class QmlBindingPath;
class QmlBindingChannel;
class CodeCompletionModel;

class LV_EDITQMLJS_EXPORT QmlEditFragment : public QObject{

    Q_OBJECT
    Q_PROPERTY(QObject*             visualParent READ visualParent WRITE setVisualParent NOTIFY visualParentChanged)
    Q_PROPERTY(int                  refCount     READ refCount     NOTIFY refCountChanged)
    Q_PROPERTY(Location             location     READ location     CONSTANT)
    Q_PROPERTY(QJSValue             whenBinding  READ whenBinding  WRITE setWhenBinding NOTIFY whenBindingChanged)
    Q_PROPERTY(lv::CodeQmlHandler*  codeHandler  READ codeHandler  CONSTANT)
    Q_PROPERTY(bool                 isNull       READ isNull       NOTIFY isNullChanged)
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

    enum FragmentType {
        Builder = 1,
        ReadOnly = 2,
        Group = 4,
        GroupChild = 8
    };

    Q_ENUM(Location)
public:
    QmlEditFragment(QmlDeclaration::Ptr declaration, lv::CodeQmlHandler* codeHandler, QObject* parent = nullptr);
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
    void addNestedObjectInfo(QVariantMap& info);
    void clearNestedObjectsInfo();
    void setObjectInfo(QVariantMap& info);

    QJSValue& whenBinding();
    void setWhenBinding(const QJSValue& whenBinding);

    void setChannel(QSharedPointer<QmlBindingChannel> channel);
    const QSharedPointer<QmlBindingChannel> channel() const;

    QSharedPointer<QmlBindingPath> fullBindingPath();

    Location location() const;

public slots:
    int fragmentType() const;
    bool isOfFragmentType(FragmentType type) const;
    void addFragmentType(FragmentType type);
    void removeFragmentType(FragmentType type);

    const QList<lv::QmlEditFragment*> childFragments();

    lv::CodeQmlHandler* codeHandler() const;

    void commit(const QVariant& value);

    int position();
    int valuePosition() const;
    int valueLength() const;
    int length() const;

    bool isBuilder() const;
    void rebuild();

    bool isGroup() const;
    void checkIfGroup();

    QString defaultValue() const;
    QString readValueText() const;

    int totalPalettes() const;
    lv::QmlEditFragment* parentFragment();
    lv::CodePalette* bindingPalette();
    QStringList bindingPath();

    void __selectedChannelChanged();

    QString type() const;
    QString typeName() const;
    QString identifier() const;
    QString objectInitializerType() const;

    QList<QObject*> paletteList() const;
    QList<QObject*> getChildFragments() const;

    void updateValue();
    void __inputRunnableObjectReady();

    QVariantList nestedObjectsInfo();
    QVariantMap  objectInfo();

    void signalPropertyAdded(lv::QmlEditFragment* ef, bool expandDefault = true);
    void signalObjectAdded(lv::QmlEditFragment* ef, QPointF p = QPointF());
    void incrementRefCount();
    void decrementRefCount();
    int refCount();

    void removeChildFragment(QmlEditFragment* edit);
    void setObjectId(QString id);
    QString objectId();

    void writeProperties(const QJSValue& properties);
    void write(const QJSValue options);
    void writeCode(const QString& code);

    QObject* readObject();
    QObject* propertyObject();

    QVariant parse();
    void updateBindings();

    void updateFromPalette();

    void suggestionsForExpression(const QString& expression, lv::CodeCompletionModel* model, bool suggestFunctions);
    bool bindExpression(const QString& expression);
    bool bindFunctionExpression(const QString& expression);

    bool isNull();
    bool isMethod();

    void __channelObjectErased();

signals:
    void visualParentChanged();
    void connectionChanged(int index);
    void aboutToRemovePalette(lv::CodePalette* palette);
    void aboutToBeRemoved();
    void paletteListEmpty();

    void objectAdded(lv::QmlEditFragment* obj, QPointF cursorCoords);
    void propertyAdded(lv::QmlEditFragment* ef, bool expandDefault);

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
    QVariantList            m_nestedObjectsInfo;
    QVariantMap             m_objectInfo;
    int                     m_refCount;
    QString                 m_objectId;
    Location                m_location;
    QJSValue                m_whenBinding;
    int                     m_fragmentType;
    lv::CodeQmlHandler*     m_codeHandler;
};

inline CodePalette *QmlEditFragment::bindingPalette(){
    return m_bindingPalette;
}

inline lv::CodeQmlHandler *QmlEditFragment::codeHandler() const {
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

inline QmlEditFragment::Location QmlEditFragment::location() const{
    return m_location;
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
