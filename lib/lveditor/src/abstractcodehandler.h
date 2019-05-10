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

#ifndef LVABSTRACTCODEHANDLER_H
#define LVABSTRACTCODEHANDLER_H

#include <QObject>

#include "live/lveditorglobal.h"
#include "live/codecompletionmodel.h"
#include "live/editorsettingscategory.h"

class QTextDocument;
class QTextCursor;
class QTextBlock;

namespace lv{

class ProjectDocument;

class LV_EDITOR_EXPORT AbstractCodeHandler : public QObject{

    Q_OBJECT

public:
    explicit AbstractCodeHandler(QObject* parent = 0);
    virtual ~AbstractCodeHandler();

    virtual void assistCompletion(
        const QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        CodeCompletionModel* model,
        QTextCursor& cursorChange
    ) = 0;

    virtual void setDocument(ProjectDocument* document) = 0;
    virtual void documentContentsChanged(int position, int charsRemoved, int charsAdded) = 0;

    virtual void rehighlightBlock(const QTextBlock &block) = 0;

    virtual QPair<int, int> contextBlock(int position) = 0;
};

}// namespace

#endif // LVABSTRACTCODEHANDLER_H
