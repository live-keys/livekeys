#ifndef QDOCUMENTQMLINFO_H
#define QDOCUMENTQMLINFO_H

#include "qqmljsparserglobal.h"
#include "qdocumentqmlobject.h"
#include <QSharedPointer>
#include <QScopedPointer>
#include <QMap>

class QTextDocument;
class QTextCursor;

namespace QmlJS{ class Value; }

namespace lcv{

class QDocumentQmlInfoPrivate;
class Q_QMLJSPARSER_EXPORT QDocumentQmlInfo{

    Q_DISABLE_COPY(QDocumentQmlInfo)

public:
    class ValueReference{
    public:
        ValueReference(const QmlJS::Value* val, QDocumentQmlInfo* p)
            : value(val)
            , parent(p)
        {}
        ValueReference() : value(0), parent(0){}

        const QmlJS::Value* value;
        QDocumentQmlInfo* parent;
    };

    enum Dialect{
        Unknown = 0,
        Javascript,
        Qml,
        QmlTypeInfo,
        QmlProject,
        Json
    };

    typedef QSharedPointer<const QDocumentQmlInfo> Ptr;
    typedef QSharedPointer<QDocumentQmlInfo> MutablePtr;

    static Dialect extensionToDialect(const QString& extension);

protected:
    QDocumentQmlInfo(const QString& fileName);

public:
    static MutablePtr create(const QString& fileName);

    QStringList extractIds();
    const ValueReference rootObject();
    const ValueReference valueForId(const QString& id);
    const QDocumentQmlObject extractValueObject(const ValueReference& value, ValueReference *parent = 0);

    void createRanges(QTextDocument* document);
    const ValueReference valueAtCursor(const QTextCursor& cursor);

    bool isValueNull(const ValueReference &vr);

    bool parse(const QString& source);

    ~QDocumentQmlInfo();

private:
    QScopedPointer<QDocumentQmlInfoPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QDocumentQmlInfo)
};

}// namespace

#endif // QDOCUMENTQMLINFO_H
