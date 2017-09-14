/****************************************************************************
**
** Copyright (C) 2014-2017 Dinu SV.
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

#ifndef QDOCUMENTQMLINFO_H
#define QDOCUMENTQMLINFO_H

#include "qqmljsparserglobal.h"
#include "qdocumentqmlobject.h"
#include "qdocumentqmlvalueobjects.h"
#include "live/qcodedeclaration.h"
#include <QQmlProperty>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QMap>

class QTextDocument;
class QTextCursor;

namespace QmlJS{
    class Value;
    class Bind;
}

namespace lcv{

class QProjectDocument;
class QCodeRuntimeBinding;
class QDocumentQmlInfoPrivate;
class Q_QMLJSPARSER_EXPORT QDocumentQmlInfo{

    Q_DISABLE_COPY(QDocumentQmlInfo)

public:
    class ValueReference{
    public:
        ValueReference(const QmlJS::Value* val, const QDocumentQmlInfo* p)
            : value(val)
            , parent(p)
        {}
        ValueReference() : value(0), parent(0){}

        const QmlJS::Value* value;
        const QDocumentQmlInfo* parent;
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

    typedef QSharedPointer<const QDocumentQmlInfo> ConstPtr;
    typedef QSharedPointer<QDocumentQmlInfo>       Ptr;

    static Dialect extensionToDialect(const QString& extension);

protected:
    QDocumentQmlInfo(const QString& fileName);

public:
    static Ptr create(const QString& fileName);

    QStringList extractIds() const;
    const ValueReference rootObject();
    const ValueReference valueForId(const QString& id) const;
    QDocumentQmlObject extractValueObject(const ValueReference& value, ValueReference *parent = 0) const;
    QString extractTypeName(const ValueReference& value) const;

    void createRanges();
    const ValueReference valueAtPosition(int position) const;
    const ValueReference valueAtPosition(int position, int& begin, int& end) const;

    bool isValueNull(const ValueReference &vr) const;

    bool isParsedCorrectly() const;
    bool parse(const QString& source);

    QmlJS::Bind* internalBind();

    static bool isObject(const QString& typeString);

    QString path() const;
    QString componentName() const;

    QDocumentQmlValueObjects::Ptr createObjects() const;

    static void syncBindings(const QString& source, QProjectDocument* document, QObject* root);
    static void syncBindings(
        const QString& source,
        QProjectDocument *document,
        QList<QCodeRuntimeBinding*> bindings,
        QObject* root
    );
    static QQmlProperty findMatchingProperty(
        const QString& source,
        QProjectDocument* document,
        QCodeDeclaration::Ptr declaration,
        QObject* root
    );

    ~QDocumentQmlInfo();

private:
    QScopedPointer<QDocumentQmlInfoPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QDocumentQmlInfo)
};

}// namespace

#endif // QDOCUMENTQMLINFO_H
