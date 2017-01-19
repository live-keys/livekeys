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

#ifndef QABSTRACTCODEHANDLER_H
#define QABSTRACTCODEHANDLER_H

#include <QObject>
#include "qlcveditorglobal.h"
#include "qcodecompletionmodel.h"

class QTextDocument;
class QTextCursor;

namespace lcv{

class QProjectDocument;

class Q_LCVEDITOR_EXPORT QAbstractCodeHandler : public QObject{

    Q_OBJECT

public:
    explicit QAbstractCodeHandler(QObject* parent = 0);
    virtual ~QAbstractCodeHandler();

    virtual void setTarget(QTextDocument* target) = 0;
    virtual void assistCompletion(
        const QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        QCodeCompletionModel* model,
        QTextCursor& cursorChange
    ) = 0;
    virtual void setDocument(QProjectDocument* document) = 0;
    virtual void updateScope(const QString& data) = 0;
};

}// namespace

#endif // QABSTRACTCODEHANDLER_H
