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
    /**
     * \brief Indicates what triggers the content change
     */
    enum ContentsTrigger{
        Engine,
        Silent
    };

public:
    explicit AbstractCodeHandler(QObject* parent = 0);
    virtual ~AbstractCodeHandler();

    /**
     * \brief Assists code completion when typing
     */
    virtual void assistCompletion(
        const QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        CodeCompletionModel* model,
        QTextCursor& cursorChange
    ) = 0;

    /**
     * \brief Sets the document being worked on
     */
    virtual void setDocument(ProjectDocument* document) = 0;
    /**
     * \brief Define on-change behaviour
     */
    virtual ContentsTrigger documentContentsChanged(int position, int charsRemoved, int charsAdded) = 0;

    /**
     * \brief Block highlighting implementation
     */
    virtual void rehighlightBlock(const QTextBlock &block) = 0;
    /**
     * \brief Returns borders of the block containing the current position
     */
    virtual QPair<int, int> contextBlock(int position) = 0;
};

}// namespace

#endif // LVABSTRACTCODEHANDLER_H
