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
    int valuePosition() const;
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
    int m_valuePosition;
    int m_valueEnd;

    QmlJS::Scanner m_scanner;

    QProjectDocument* m_document;
};

inline int QDocumentQmlValueScanner::colonPosition() const{
    return m_colonPosition;
}

inline int QDocumentQmlValueScanner::valuePosition() const{
    return m_valuePosition;
}

inline int QDocumentQmlValueScanner::valueEnd() const{
    return m_valueEnd;
}

inline int QDocumentQmlValueScanner::valueLength() const{
    return m_valueEnd - m_colonPosition - 1;
}

}// namespace

#endif // QDOCUMENTQMLVALUESCANNER_H
