#ifndef QDOCUMENTQMLVALUESCANNER_H
#define QDOCUMENTQMLVALUESCANNER_H

#include "qprojectdocument.h"
#include "qmljs/qmljsscanner.h"

class QTextDocument;

namespace lcv{

class QDocumentQmlValueScanner{

public:
    QDocumentQmlValueScanner(QProjectDocument* document, int position, int length);
    ~QDocumentQmlValueScanner();

    bool operator()();

    int colonPosition() const;
    int valueEnd() const;
    int valueLength() const;

    static int getPropertyLength(
        QTextDocument* document,
        int position,
        QStringList* propertyPath = 0
    );


private:
    int findColonInTokenSet(const QList<QmlJS::Token>& tokens);

    int m_position;
    int m_length;

    int m_colonPosition;
    int m_valueEnd;

    QmlJS::Scanner m_scanner;

    QProjectDocument* m_document;
};

inline int QDocumentQmlValueScanner::colonPosition() const{
    return m_colonPosition;
}

inline int QDocumentQmlValueScanner::valueEnd() const{
    return m_valueEnd;
}

inline int QDocumentQmlValueScanner::valueLength() const{
    return m_valueEnd - m_colonPosition - 1;
}

}// namespace

#endif // QDOCUMENTQMLVALUESCANNER_H
