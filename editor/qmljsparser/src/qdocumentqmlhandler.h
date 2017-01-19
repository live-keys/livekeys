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

#ifndef QDOCUMENTQMLHANDLER_H
#define QDOCUMENTQMLHANDLER_H

#include "qqmljsparserglobal.h"
#include "qabstractcodehandler.h"
#include "qprojectdocument.h"
#include "qdocumentqmlscope.h"
#include "qprojectqmlscope.h"
#include "qlockedfileiosession.h"

#include <QTextCursor>

class QQmlEngine;

class QQmlJsHighlighter;

namespace lcv{

class QProjectQmlScanner;
class QPluginInfoExtractor;
class QQmlCompletionContextFinder;
class QQmlCompletionContext;

class Q_QMLJSPARSER_EXPORT QDocumentQmlHandler : public QAbstractCodeHandler{

    Q_OBJECT
    Q_DISABLE_COPY(QDocumentQmlHandler)

public:
    explicit QDocumentQmlHandler(
        QQmlEngine* engine,
        QMutex* engineMutex,
        QLockedFileIOSession::Ptr lockedFileIO,
        QObject* parent = 0
    );
    ~QDocumentQmlHandler();

    void assistCompletion(
        const QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        QCodeCompletionModel* model,
        QTextCursor& cursorChange
    ) Q_DECL_OVERRIDE;
    void setTarget(QTextDocument *target) Q_DECL_OVERRIDE;
    void setDocument(QProjectDocument* document) Q_DECL_OVERRIDE;
    void updateScope(const QString& data) Q_DECL_OVERRIDE;

    QPluginInfoExtractor *getPluginInfoExtractor(const QString& import);

public slots:
    void newDocumentScopeReady();
    void newProjectScope();
    void newProject(const QString& path);
    void directoryChanged(const QString& path);
    void fileChanged(const QString& path);
    void loadImport(const QString& import);

private:
    void suggestionsForGlobalQmlContext(
        const QQmlCompletionContext& context,
        QList<QCodeCompletionSuggestion>& suggestions
    );

    void suggestionsForImport(
        const QQmlCompletionContext& context,
        QList<QCodeCompletionSuggestion>& suggestions
    );
    void suggestionsForStringImport(
        const QString &enteredPath,
        QList<QCodeCompletionSuggestion>& suggestions,
        QString &filter
    );
    void suggestionsForRecursiveImport(
        int index,
        const QString &dir,
        const QStringList& expression,
        QList<QCodeCompletionSuggestion>& suggestions
    );

    void suggestionsForValueObject(
        const QDocumentQmlObject& object,
        QList<QCodeCompletionSuggestion>& suggestions,
        bool extractProperties,
        bool extractFunctions,
        bool extractSlots,
        bool extractSignals
    );

    void suggestionsForNamespaceTypes(
        const QString& typeNameSpace,
        QList<QCodeCompletionSuggestion>& suggestions
    );
    void suggestionsForNamespaceImports(
        QList<QCodeCompletionSuggestion>& suggestions
    );
    void suggestionsForDocumentsIds(
        QList<QCodeCompletionSuggestion>& suggestions
    );

    void suggestionsForLeftBind(
        const QQmlCompletionContext& context,
        int cursorPosition,
        QList<QCodeCompletionSuggestion>& suggestions
    );
    void suggestionsForRightBind(
        const QQmlCompletionContext& context,
        int cursorPosition,
        QList<QCodeCompletionSuggestion>& suggestions
    );
    void suggestionsForLeftSignalBind(
        const QQmlCompletionContext& context,
        int cursorPosition,
        QList<QCodeCompletionSuggestion>& suggestions
    );

    QString extractQuotedString(const QTextCursor& cursor) const;

    QTextDocument*      m_target;
    QQmlJsHighlighter*  m_highlighter;
    QQmlEngine*         m_engine;
    QQmlCompletionContextFinder* m_completionContextFinder;

    QProjectDocument* m_document;

    QDocumentQmlScope::Ptr       m_documentScope;
    QProjectQmlScope::Ptr m_projectScope;
    bool                         m_newScope;
    QProjectQmlScanner*          m_scanner;

};

}// namespace

#endif // QDOCUMENTQMLHANDLER_H
