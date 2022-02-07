/****************************************************************************
**
** Copyright (C) 2022 Dinu SV.
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

#ifndef LVDOCUMENTQMLVALUESCANNER_H
#define LVDOCUMENTQMLVALUESCANNER_H

#include "live/projectdocument.h"
#include "qmljs/qmljsscanner.h"

class QTextDocument;

namespace lv{

/// \private
class DocumentQmlValueScanner{

public:
    DocumentQmlValueScanner(ProjectDocument* document, int position, int length);
    ~DocumentQmlValueScanner();

    bool operator()();

    int colonPosition() const;
    int valuePosition() const;
    int valueEnd() const;
    int valueLength() const;

    static int getExpressionExtent(
        QTextDocument* document,
        int position,
        QStringList* propertyPath = nullptr,
        QChar *endDelimiter = nullptr);

    static int getPropertyExpressionExtent(
        QTextDocument* document,
        int position,
        QStringList* propertyPath = nullptr,
        QChar *endDelimiter = nullptr,
        int* newPosition = nullptr);

    int getBlockStart(int position);
    int getBlockEnd(int position);
    int getBlockExtent(int from);

    static int getBlockEnd(QTextBlock& bl, int position);

private:
    int findColonInTokenSet(const QList<QQmlJS::Token>& tokens);

    int m_position;
    int m_length;

    int m_colonPosition;
    int m_valuePosition;
    int m_valueEnd;

    QQmlJS::Scanner m_scanner;

    ProjectDocument* m_document;
};

inline int DocumentQmlValueScanner::colonPosition() const{
    return m_colonPosition;
}

inline int DocumentQmlValueScanner::valuePosition() const{
    return m_valuePosition;
}

inline int DocumentQmlValueScanner::valueEnd() const{
    return m_valueEnd;
}

inline int DocumentQmlValueScanner::valueLength() const{
    return m_valueEnd - m_colonPosition - 1;
}

}// namespace

#endif // LVDOCUMENTQMLVALUESCANNER_H
