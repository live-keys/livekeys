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

#ifndef LVDOCUMENTQMLINFO_H
#define LVDOCUMENTQMLINFO_H

#include "live/lveditqmljsglobal.h"
#include "live/document.h"
#include "live/documentqmlobject.h"
#include "live/documentqmlvalueobjects.h"
#include "live/qmldeclaration.h"
#include "live/qmllanguageinfo.h"

#include <QQmlProperty>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QMap>

class QTextDocument;
class QTextCursor;

namespace QmlJS{
    class Value;
    class Bind;
    namespace AST{ class Node; }
}

namespace lv{

class ProjectDocument;
class Runnable;
class QmlBindingPath;
class QmlBindingChannel;
class DocumentQmlInfoPrivate;


class LV_EDITQMLJS_EXPORT DocumentQmlInfo{

    Q_DISABLE_COPY(DocumentQmlInfo)

public:
    class LV_EDITQMLJS_EXPORT Import{

    public:
        static const int NoVersion;

        /** Import type */
        enum Type{
            /** Invalid type */
            Invalid,
            /** Library based import with qmldir file. */
            Library,
            /** Directory based import */
            Directory,
            /** Implicit import */
            ImplicitDirectory,
            /** Js file import */
            File,
            /** Unknown file import */
            UnknownFile
        };

        Import(
            Type importType,
            const QString& uri,
            const QString& as = "",
            int vMajor = NoVersion,
            int vMinor = NoVersion,
            Document::Location location = Document::Location()
        );
        Import(
            Type importType,
            const QString& uri,
            const QString& relativeUri,
            const QString& as = "",
            int vMajor = NoVersion,
            int vMinor = NoVersion,
            Document::Location location = Document::Location()
        );

        void updateUri(const QString& uri);
        void setImportType(Type importType);

        Type importType() const;

        const QString& uri() const;
        const QString& relativeUri() const;
        const QString& as() const;
        int versionMajor() const;
        int versionMinor() const;
        bool isVersionValid() const;
        QString toString() const;
        QString versionString() const;
        void setLocation(const Document::Location& location);
        const Document::Location& location() const;

        static QString join(const QList<Import>& imports);

        bool operator ==(const Import& other) const;

    private:
        Type    m_type;
        QString m_uri;
        QString m_relativeUri;
        QString m_as;
        int     m_versionMajor;
        int     m_versionMinor;
        Document::Location m_location;
    };

    class TraversalResult{
    public:
        TraversalResult() : bindingPath(), range(nullptr){}

        QSharedPointer<QmlBindingPath> bindingPath;
        DocumentQmlValueObjects::RangeItem* range;
    };

    /** List of import entries */
    typedef QList<Import> ImportList;

    class ValueReference{
    public:
        /// \brief ValueReference constructor
        ValueReference(const QmlJS::Value* val, const DocumentQmlInfo* p)
            : value(val)
            , parent(p)
        {}
        /// \brief ValueReference destructor
        ValueReference() : value(nullptr), parent(nullptr){}

        const QmlJS::Value* value;
        const DocumentQmlInfo* parent;
    };

    class ASTReference{
    public:
        /// \brief ASTReference constructor
        ASTReference(QmlJS::AST::Node* n) : node(n){}
        /// \brief ASTReference destructor
        ASTReference() : node(nullptr){}

        QmlJS::AST::Node* node;
    };

    class Message{

    public:
        /// Severity of a message
        enum Severity{
            /// Lowest severity level
            Hint,
            /// Might be a warning
            MaybeWarning,
            /// Warning Level
            Warning,
            /// Might be an error
            MaybeError,
            /// Error level
            Error
        };

    public:
        Message(Severity pkind, int pposition, int pline, const QString& ptext)
            : kind(pkind)
            , position(pposition)
            , line(pline)
            , text(ptext)
        {}


        Severity kind;
        int      position;
        int      line;
        QString  text;
    };

    /// Dialect used by the source code of a file
    enum Dialect{
        /// Unknown dialect
        Unknown = 0,
        /// Javascript dialect
        Javascript,
        /// Qml dialect
        Qml,
        /// QmlTypeInfo dialect
        QmlTypeInfo,
        /// QmlProject dialect
        QmlProject,
        /// Json dialect
        Json
    };

    /// Constat SharedPointer to DocumentqmlInfo
    typedef QSharedPointer<const DocumentQmlInfo> ConstPtr;
    /// SharedPointer to DocumentqmlInfo
    typedef QSharedPointer<DocumentQmlInfo>       Ptr;

    static Dialect extensionToDialect(const QString& extension);

protected:
    DocumentQmlInfo(const QString& fileName);

public:
    static Ptr create(const QString& fileName);
    static Ptr createAndParse(const QString& fileName, const QString& source);

    QStringList extractIds() const;
    const ValueReference rootObject();
    const ValueReference valueForId(const QString& id) const;
    lv::QmlTypeInfo::Ptr extractValueObject(const ValueReference& value, ValueReference *parent = nullptr) const;
    lv::QmlTypeInfo::Ptr extractValueObjectWithExport(
            const ValueReference& value,
            const QString& componentName,
            const QString& libraryPath,
            bool isSingleton = false) const;
    QStringList extractTypeName(const ValueReference& value) const;
    void extractTypeNameRange(const ValueReference& value, int& begin, int& end) const;
    void extractRange(const ValueReference& value, int& begin, int& end);

    void createRanges();
    const ValueReference valueAtPosition(int position) const;
    const ValueReference valueAtPosition(int position, int& begin, int& end) const;
    const ASTReference astObjectAtPosition(int position);

    QString propertySourceFromObjectId(const QString& componentId, const QString& propertyName);

    bool isValueNull(const ValueReference &vr) const;

    bool isParsedCorrectly() const;
    bool parse(const QString& source);
    const QList<DocumentQmlInfo::Message>& diagnostics() const;

    QmlJS::Bind* internalBind();

    QString path() const;
    QString componentName() const;

    QString source() const;

    void tryExtractImports();

    DocumentQmlValueObjects::Ptr createObjects() const;
    DocumentQmlValueObjects::Ptr createObjects(const ASTReference& ast) const;

    static TraversalResult findDeclarationPath(
            ProjectDocument* document,
            DocumentQmlValueObjects::RangeObject *object,
            QmlDeclaration::Ptr declaration);

    ~DocumentQmlInfo();

    void updateImportType(const QString& uri, Import::Type type);
    bool hasImport(const Import& key) const;
    bool hasImport(const QString& importUri) const;
    bool hasImportAs(const QString& asKey) const;
    int totalImports() const;
    const ImportList& imports() const;

    void transferImports(const ImportList& imports);
    void addImport(const Import& key);

private:
    QScopedPointer<DocumentQmlInfoPrivate> d_ptr;
    QList<Import> extractImports();

    Q_DECLARE_PRIVATE(DocumentQmlInfo)
};


/// \brief Returns the major version of this import
inline int DocumentQmlInfo::Import::versionMajor() const{
    return m_versionMajor;
}

/// \brief Returns the minor version of this import
inline int DocumentQmlInfo::Import::versionMinor() const{
    return m_versionMinor;
}

/// \brief Returns true if this version is valid, false otherwise
inline bool DocumentQmlInfo::Import::isVersionValid() const{
    return m_versionMajor >= 0 && m_versionMinor >= 0;
}

/// \brief Returns true if this object is the same as \p other, false otherwise
inline bool DocumentQmlInfo::Import::operator ==(const DocumentQmlInfo::Import &other) const{
    if ( uri() != other.uri() )
        return false;
    if ( versionMajor() != other.versionMajor() )
        return false;
    return versionMinor() == other.versionMinor();
}

/// \brief Returns the path of this import
inline const QString &DocumentQmlInfo::Import::uri() const{
    return m_uri;
}

/// \brief Updates the import uri for this import
inline void DocumentQmlInfo::Import::updateUri(const QString &uri){
    m_uri = uri;
}

inline void DocumentQmlInfo::Import::setImportType(DocumentQmlInfo::Import::Type importType){
    m_type = importType;
}

/// \brief Returns the import type for this import
inline DocumentQmlInfo::Import::Type DocumentQmlInfo::Import::importType() const{
    return m_type;
}

/// \brief Returns the namespace this import was declared in
inline const QString &DocumentQmlInfo::Import::as() const{
    return m_as;
}

/// \brief Returns the import relative uri.
inline const QString &DocumentQmlInfo::Import::relativeUri() const{
    return m_relativeUri;
}


}// namespace

#endif // LVDOCUMENTQMLINFO_H
