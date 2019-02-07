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

#ifndef LVDOCUMENTQMLINFO_H
#define LVDOCUMENTQMLINFO_H

#include "live/lveditqmljsglobal.h"
#include "live/documentqmlobject.h"
#include "live/documentqmlvalueobjects.h"
#include "live/qmldeclaration.h"

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
class BindingPath;
class DocumentQmlInfoPrivate;


class LV_EDITQMLJS_EXPORT DocumentQmlInfo{

    Q_DISABLE_COPY(DocumentQmlInfo)

public:
    class ValueReference{
    public:
        /// \brief ValueReference constructor
        ValueReference(const QmlJS::Value* val, const DocumentQmlInfo* p)
            : value(val)
            , parent(p)
        {}
        /// \brief ValueReference destructor
        ValueReference() : value(0), parent(0){}

        const QmlJS::Value* value;
        const DocumentQmlInfo* parent;
    };

    class ASTReference{
    public:
        /// \brief ASTReference constructor
        ASTReference(QmlJS::AST::Node* n) : node(n){}
        /// \brief ASTReference destructor
        ASTReference() : node(0){}

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

    QStringList extractIds() const;
    const ValueReference rootObject();
    const ValueReference valueForId(const QString& id) const;
    DocumentQmlObject extractValueObject(const ValueReference& value, ValueReference *parent = 0) const;
    QString extractTypeName(const ValueReference& value) const;
    void extractTypeNameRange(const ValueReference& value, int& begin, int& end);
    void extractRange(const ValueReference& value, int& begin, int& end);

    void createRanges();
    const ValueReference valueAtPosition(int position) const;
    const ValueReference valueAtPosition(int position, int& begin, int& end) const;
    const ASTReference astObjectAtPosition(int position);

    bool isValueNull(const ValueReference &vr) const;

    bool isParsedCorrectly() const;
    bool parse(const QString& source);

    QmlJS::Bind* internalBind();

    static bool isObject(const QString& typeString);
    static QString typeDefaultValue(const QString& typeString);

    QString path() const;
    QString componentName() const;

    DocumentQmlValueObjects::Ptr createObjects() const;
    DocumentQmlValueObjects::Ptr createObjects(const ASTReference& ast) const;

    static void traverseBindingPath(BindingPath* path, QObject* root);
    static BindingPath* findDeclarationPath(
            DocumentQmlValueObjects::RangeObject *object,
            QmlDeclaration::Ptr declaration);
    static BindingPath *findDeclarationPath(
        const QString& source,
        ProjectDocument* document,
        QmlDeclaration::Ptr declaration);

    ~DocumentQmlInfo();

private:
    QScopedPointer<DocumentQmlInfoPrivate> d_ptr;

    Q_DECLARE_PRIVATE(DocumentQmlInfo)
};

}// namespace

#endif // LVDOCUMENTQMLINFO_H
