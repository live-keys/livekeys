#ifndef LVQMLSCOPESNAP_H
#define LVQMLSCOPESNAP_H

#include "plugintypesfacade.h"
#include "documentqmlvaluescanner_p.h"
#include "documentqmlvalueobjects.h"
#include "qmljsbuiltintypes_p.h"
#include "qmljshighlighter_p.h"
#include "bindingchannel.h"
#include "qmlcursorinfo.h"
#include "projectqmlscope.h"
#include "projectqmlscopecontainer_p.h"
#include "documentqmlscope.h"

#include "qmljs/qmljsscanner.h"
#include "projectqmlscanner_p.h"
#include "projectqmlscanmonitor_p.h"
#include "documentqmlobject.h"
#include "documentqmlobject_p.h"

namespace lv{

/// \private
class QmlScopeSnap{

public:
    /// \private
    class LibraryReference{
    public:
        LibraryReference();
        LibraryReference(const QString& libPath, QmlLibraryInfo::Ptr libInfo);

        bool isValid() const;

    public:
        QString importUri;
        int     versionMajor;
        int     versionMinor;
        QString path;
    };

    /// \private
    class TypeReference{
    public:
        TypeReference();
        TypeReference(const LibraryReference& lib, LanguageUtils::FakeMetaObject::ConstPtr obj);

        bool isValid() const;
        QString toString() const;

    public:
        QString typeName;
        LibraryReference library;
        LanguageUtils::FakeMetaObject::ConstPtr object;
        QString qmlFile;
    };

    /// \private
    class InheritancePath{
    public:
        QString toString() const;
        void join(const InheritancePath& path);
        void append(const TypeReference& tr);
        bool isEmpty() const;

    public:
        QList<TypeReference> nodes;
    };

    /// \private
    class PropertyReference{
    public:
        PropertyReference();

        PropertyReference(
            const LanguageUtils::FakeMetaProperty& property,
            bool isClassName,
            const InheritancePath& propertyType,
            const InheritancePath& classType);

        bool isValid() const;
        QString toString() const;

        static QString toString(const QList<PropertyReference>& propertyChain);

    public:
        LanguageUtils::FakeMetaProperty property;
        bool isPrimitive;
        bool typeIsClassName;
        InheritancePath propertyTypePath;
        InheritancePath classTypePath;
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

        InheritancePath typeReference;
        bool isClass;
        bool isId;
        bool isParent;

        QString enumName;

        QList<PropertyReference> propertyChain;
        DocumentQmlObject documentValueObject;
    };

public:
    QmlScopeSnap(const ProjectQmlScope::Ptr& project, const DocumentQmlScope::Ptr& document);

    TypeReference getType(const QString& name) const;
    InheritancePath getTypePath(const QString& name) const;
    InheritancePath getTypePath(const QStringList& typeAndImport) const;

    TypeReference getType(const QString& importNamespace, const QString& typeName) const;
    InheritancePath getTypePath(const QString& importAs, const QString& name) const;

    InheritancePath generateTypePathFromObject(const TypeReference& tr) const;
    InheritancePath generateTypePathFromClassName(const QString& typeName, QString typeLibrary) const;

    InheritancePath propertyTypePath(const InheritancePath& classTypePath, const QString& propertyName) const;
    PropertyReference propertyInType(const InheritancePath& classTypePath, const QString& propertyName) const;

    LanguageUtils::FakeMetaProperty getPropertyInObject(
        const QmlScopeSnap::InheritancePath& typePath,
        const QString& propertyName
    ) const;

    PropertyReference getProperty(const QStringList& contextObject, const QString& propertyName, int position) const;
    QList<PropertyReference> getProperties(const QStringList& context, const QStringList& propertyChain, int position) const;
    QList<PropertyReference> getProperties(const InheritancePath& typePath, const QStringList& propertyChain) const;

    ExpressionChain evaluateExpression(const QStringList& contextObject, const QStringList& expression, int position) const;

    bool isImport(const QString& name) const;
    bool isEnum(const InheritancePath& classTypePath, const QString& name) const;

public:
    ProjectQmlScope::Ptr  project;
    DocumentQmlScope::Ptr document;
};

}// namespace

#endif // LVQMLSCOPESNAP_H
