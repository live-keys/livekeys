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

#ifndef LVCODEQMLHANDLER_H
#define LVCODEQMLHANDLER_H

#include "live/lveditqmljsglobal.h"
#include "live/abstractcodehandler.h"
#include "live/projectdocument.h"
#include "live/documentqmlscope.h"
#include "live/projectqmlscope.h"
#include "live/codedeclaration.h"
#include "live/lockedfileiosession.h"
#include "live/viewengine.h"
#include "live/settings.h"

#include <QTextCursor>

class QQmlEngine;

namespace lv{

class ProjectQmlScanner;
class ProjectQmlExtension;
class PluginInfoExtractor;
class QmlJsHighlighter;
class QmlCompletionContextFinder;
class QmlCompletionContext;
class QmlJsSettings;

class LV_EDITQMLJS_EXPORT CodeQmlHandler : public AbstractCodeHandler{

    Q_OBJECT
    Q_DISABLE_COPY(CodeQmlHandler)

public:
    explicit CodeQmlHandler(
        ViewEngine* engine,
        Project* project,
        QmlJsSettings *settings,
        ProjectQmlExtension* projectHandler,
        QObject* parent = 0
    );
    ~CodeQmlHandler();

    void assistCompletion(
        const QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        CodeCompletionModel* model,
        QTextCursor& cursorChange
    ) Q_DECL_OVERRIDE;
    void setTarget(QTextDocument *target) Q_DECL_OVERRIDE;
    void setDocument(ProjectDocument* document) Q_DECL_OVERRIDE;
    void updateScope(const QString& data) Q_DECL_OVERRIDE;
    void rehighlightBlock(const QTextBlock& block) Q_DECL_OVERRIDE;
    QList<lv::CodeDeclaration::Ptr> getDeclarations(const QTextCursor& cursor) Q_DECL_OVERRIDE;
    bool findDeclarationValue(int position, int length, int& valuePosition, int& valueEnd) Q_DECL_OVERRIDE;
    void connectBindings(QList<CodeRuntimeBinding*> bindings, QObject* root) Q_DECL_OVERRIDE;
    DocumentEditFragment* createInjectionChannel(
        CodeDeclaration::Ptr property,
        QObject* runtime,
        CodeConverter* converter
    ) Q_DECL_OVERRIDE;
    QPair<int, int> contextBlock(int position) Q_DECL_OVERRIDE;


    QmlJsSettings* settings();

public slots:
    void newDocumentScopeReady(const QString& path, DocumentQmlScope::Ptr documentScope);
    void newProjectScopeReady();

private:
    void suggestionsForGlobalQmlContext(
        const QmlCompletionContext& context,
        QList<CodeCompletionSuggestion>& suggestions
    );

    void suggestionsForImport(
        const QmlCompletionContext& context,
        QList<CodeCompletionSuggestion>& suggestions
    );
    void suggestionsForStringImport(
        const QString &enteredPath,
        QList<CodeCompletionSuggestion>& suggestions,
        QString &filter
    );
    void suggestionsForRecursiveImport(
        int index,
        const QString &dir,
        const QStringList& expression,
        QList<CodeCompletionSuggestion>& suggestions
    );

    void suggestionsForValueObject(
        const DocumentQmlObject& object,
        QList<CodeCompletionSuggestion>& suggestions,
        bool extractProperties,
        bool extractFunctions,
        bool extractSlots,
        bool extractSignals
    );

    void suggestionsForNamespaceTypes(
        const QString& typeNameSpace,
        QList<CodeCompletionSuggestion>& suggestions
    );
    void suggestionsForNamespaceImports(
        QList<CodeCompletionSuggestion>& suggestions
    );
    void suggestionsForDocumentsIds(
        QList<CodeCompletionSuggestion>& suggestions
    );

    void suggestionsForLeftBind(
        const QmlCompletionContext& context,
        int cursorPosition,
        QList<CodeCompletionSuggestion>& suggestions
    );
    void suggestionsForRightBind(
        const QmlCompletionContext& context,
        int cursorPosition,
        QList<CodeCompletionSuggestion>& suggestions
    );
    void suggestionsForLeftSignalBind(
        const QmlCompletionContext& context,
        int cursorPosition,
        QList<CodeCompletionSuggestion>& suggestions
    );

    QString extractQuotedString(const QTextCursor& cursor) const;

    QTextDocument*      m_target;
    QmlJsHighlighter*   m_highlighter;
    QmlJsSettings*      m_settings;
    QQmlEngine*         m_engine;
    QmlCompletionContextFinder* m_completionContextFinder;

    ProjectDocument*       m_document;

    DocumentQmlScope::Ptr  m_documentScope;
    ProjectQmlExtension*   m_projectHandler;
    bool                   m_newScope;

};

inline QmlJsSettings *CodeQmlHandler::settings(){
    return m_settings;
}

}// namespace

#endif // LVCODEQMLHANDLER_H
