#ifndef QDOCUMENTQMLINFO_H
#define QDOCUMENTQMLINFO_H

#include "qqmljsparserglobal.h"
#include <QSharedPointer>
#include <QScopedPointer>
#include <QMap>

namespace QmlJS{ class Value; }

namespace lcv{

class QDocumentQmlInfoPrivate;
class Q_QMLJSPARSER_EXPORT QDocumentQmlInfo{

    Q_DISABLE_COPY(QDocumentQmlInfo)

public:
    class ValueObject{
    public:
        QString className;
        QMap<QString, QString> members;
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
    const QmlJS::Value *valueForId(const QString& id);
    const ValueObject extractValueData(const QmlJS::Value *value, const QmlJS::Value** parent = 0);

    bool parse(const QString& source);

    ~QDocumentQmlInfo();

private:
    QScopedPointer<QDocumentQmlInfoPrivate> d_ptr;

    Q_DECLARE_PRIVATE(QDocumentQmlInfo)
};

}// namespace

#endif // QDOCUMENTQMLINFO_H
