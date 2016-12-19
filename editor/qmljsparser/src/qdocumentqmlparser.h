#ifndef QDOCUMENTQMLPARSER_H
#define QDOCUMENTQMLPARSER_H

#include "qqmljsparserglobal.h"
#include "qdocumentqmlinfo.h"
#include <QString>

namespace lcv{

class Q_QMLJSPARSER_EXPORT QDocumentQmlParser{

public:
    QDocumentQmlParser();
    ~QDocumentQmlParser();

    QDocumentQmlInfo::MutablePtr operator()(const QString& filePath, const QString& text);

};

}// namespace

#endif // QDOCUMENTQMLPARSER_H
