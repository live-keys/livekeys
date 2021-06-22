#ifndef LVQMLSCOPESNAP_H
#define LVQMLSCOPESNAP_H

#include "plugintypesfacade.h"
#include "documentqmlvaluescanner_p.h"
#include "documentqmlvalueobjects.h"
#include "qmljsbuiltintypes_p.h"
#include "qmljshighlighter_p.h"
#include "qmlbindingchannel.h"
#include "projectqmlscope.h"

#include "qmljs/qmljsscanner.h"
#include "qmlprojectmonitor_p.h"
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
        QmlTypeInfo::Ptr documentValueObject;
    };

public:
    QmlScopeSnap(const ProjectQmlScope::Ptr& project, const DocumentQmlInfo::Ptr& document);

    bool areDocumentLibrariesReady() const;

    QmlTypeInfo::Ptr getType(const QString& name) const;
    QmlTypeInfo::Ptr getType(const QString& importNamespace, const QString& typeName) const;
    QmlTypeInfo::Ptr getType(const QStringList& typeAndImport) const;
    QmlTypeInfo::Ptr getType(const QmlTypeReference& typeReference) const;
    QmlTypeInfo::Ptr getTypeFromContextLibrary(
        const QString& typeName,
        const QString& libraryUri,
        QmlTypeReference::Language language = QmlTypeReference::Cpp) const;

    QmlInheritanceInfo getTypePath(const QString& name) const;
    QmlInheritanceInfo getTypePath(const QStringList& typeAndImport) const;
    QmlInheritanceInfo getTypePath(const QString& importAs, const QString& name) const;
    QmlInheritanceInfo getTypePath(const QmlTypeReference& languageType) const;

    QmlLibraryInfo::Ptr libraryFromUri(const QString& uri) const;
    QmlLibraryInfo::Ptr libraryFromType(const QmlTypeReference& tr) const;

    QmlInheritanceInfo generateTypePathFromObject(const QmlTypeInfo::Ptr &tr) const;
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
    ProjectQmlScope::Ptr project;
    DocumentQmlInfo::Ptr document;
};

}// namespace

#endif // LVQMLSCOPESNAP_H
