#ifndef LVQMLSCOPESNAP_H
#define LVQMLSCOPESNAP_H

#include "plugintypesfacade.h"
#include "documentqmlvaluescanner_p.h"
#include "documentqmlvalueobjects.h"
#include "qmljsbuiltintypes_p.h"
#include "qmljshighlighter_p.h"
#include "qmlbindingchannel.h"
#include "qmlcursorinfo.h"
#include "projectqmlscope.h"
#include "projectqmlscopecontainer_p.h"
#include "documentqmlscope.h"

#include "qmljs/qmljsscanner.h"
#include "projectqmlscanner_p.h"
#include "projectqmlscanmonitor_p.h"
#include "qmllanguageinfo.h"
#include "qmllanguageinfo_p.h"

namespace lv{

/// \private
class QmlScopeSnap{

public:
    /// \private
    class PropertyReference{
    public:
        PropertyReference();

        PropertyReference(
            const QmlPropertyInfo& property,
            bool isClassName,
            const QmlInheritanceInfo& resultType,
            const QmlInheritanceInfo& classType);

        PropertyReference(
            const QmlFunctionInfo& property,
            bool isClassName,
            const QmlInheritanceInfo& resultType,
            const QmlInheritanceInfo& classType);

        bool isValid() const;
        QString toString() const;

        static QString toString(const QList<PropertyReference>& propertyChain);

        QmlTypeReference resultType() const;
        QmlTypeReference propertyObjectType() const;

    public:
        QmlPropertyInfo property;
        QmlFunctionInfo functionInfo;
        bool isPrimitive;
        bool typeIsClassName;
        QmlInheritanceInfo resultTypePath;
        QmlInheritanceInfo classTypePath;
    };

    /// \private
    class ExpressionChain{
    public:
        enum NodeType{
            Invalid = 0,
            NamespaceNode,
            ClassNode,
            ObjectNode,
            EnumNode,
            PropertyNode
        };

    public:
        ExpressionChain(const QStringList& expression);

        bool isValid() const;
        bool hasNamespace() const;
        bool hasClass() const;
        bool hasObject() const;
        bool hasEnum() const;
        bool hasProperty() const;

        NodeType lastSegmentType() const;
        QString toString(bool detail = true) const;

    public:
        QStringList expression;

        QString importAs;

        QmlInheritanceInfo typeReference;
        bool isClass;
        bool isId;
        bool isParent;

        QString enumName;

        QList<PropertyReference> propertyChain;
        QmlTypeInfo documentValueObject;
    };

public:
    QmlScopeSnap(const ProjectQmlScope::Ptr& project, const DocumentQmlScope::Ptr& document);

    QmlTypeInfo getType(const QString& name) const;
    QmlTypeInfo getType(const QString& importNamespace, const QString& typeName) const;
    QmlTypeInfo getType(const QStringList& typeAndImport);
    QmlTypeInfo getTypeFromContextLibrary(
        const QString& typeName,
        const QString& libraryUri,
        QmlTypeReference::Language language = QmlTypeReference::Cpp) const;

    QmlInheritanceInfo getTypePath(const QString& name) const;
    QmlInheritanceInfo getTypePath(const QStringList& typeAndImport) const;
    QmlInheritanceInfo getTypePath(const QString& importAs, const QString& name) const;
    QmlInheritanceInfo getTypePath(const QmlTypeReference& languageType) const;

    QmlLibraryInfo::Reference libraryFromUri(const QString& uri) const;
    QmlLibraryInfo::Reference libraryFromType(const QmlTypeReference& tr) const;

    QmlInheritanceInfo generateTypePathFromObject(const QmlTypeInfo& tr) const;
    QmlInheritanceInfo generateTypePathFromClassName(const QString& typeName, QString typeLibrary) const;

    QmlInheritanceInfo propertyTypePath(const QmlInheritanceInfo& classTypePath, const QString& propertyName) const;
    PropertyReference propertyInType(const QmlInheritanceInfo& classTypePath, const QString& propertyName) const;

    PropertyReference getProperty(const QmlTypeReference &contextObject, const QString& propertyName, int position) const;
    QmlFunctionInfo getSignal(const QmlTypeReference& contextObject, const QString& signalName, int position) const;
    QList<PropertyReference> getProperties(const QmlTypeReference &context, const QStringList& propertyChain, int position) const;
    QList<PropertyReference> getProperties(const QmlInheritanceInfo& typePath, const QStringList& propertyChain) const;

    ExpressionChain evaluateExpression(const QmlTypeReference &contextObject, const QStringList& expression, int position) const;

    QString importToUri(const QString& name) const;
    QmlTypeReference quickObjectDeclarationType(const QStringList &declaration) const;
    bool isImport(const QString& name) const;
    bool isEnum(const QmlInheritanceInfo& classTypePath, const QString& name) const;

public:
    ProjectQmlScope::Ptr  project;
    DocumentQmlScope::Ptr document;
};

}// namespace

#endif // LVQMLSCOPESNAP_H
