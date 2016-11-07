#ifndef QDOCUMENTQMLINFO_H
#define QDOCUMENTQMLINFO_H

#include <QSharedPointer>

namespace lcv{

class QDocumentQmlInfo{

    Q_DISABLE_COPY(QDocumentQmlInfo)

public:
    typedef QSharedPointer<const QDocumentQmlInfo> Ptr;
    typedef QSharedPointer<QDocumentQmlInfo> MutablePtr;

protected:
    QDocumentQmlInfo();

public:
    ~QDocumentQmlInfo();
};

}// namespace

#endif // QDOCUMENTQMLINFO_H
