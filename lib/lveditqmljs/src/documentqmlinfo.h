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
        ValueReference(const QmlJS::Value* val, const DocumentQmlInfo* p)
            : value(val)
            , parent(p)
        {}
        ValueReference() : value(0), parent(0){}

        const QmlJS::Value* value;
        const DocumentQmlInfo* parent;
    };

    class ASTReference{
    public:
        ASTReference(QmlJS::AST::Node* n) : node(n){}
        ASTReference() : node(0){}

        QmlJS::AST::Node* node;
    };

    class Message{

    public:
        enum Severity{
            Hint,
            MaybeWarning,
            Warning,
            MaybeError,
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

    enum Dialect{
        Unknown = 0,
        Javascript,
        Qml,
        QmlTypeInfo,
        QmlProject,
        Json
    };

    typedef QSharedPointer<const DocumentQmlInfo> ConstPtr;
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
