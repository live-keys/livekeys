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
#include "live/qmldeclaration.h"
#include "live/lockedfileiosession.h"
#include "live/viewengine.h"
#include "live/settings.h"
#include "live/qmladdcontainer.h"
#include "live/palettelist.h"
#include "live/documentcursorinfo.h"

#include <QTimer>
#include <QTextCursor>
#include <QQmlListProperty>

class QQmlEngine;

namespace lv{

class ProjectQmlScanner;
class ProjectQmlExtension;
class PluginInfoExtractor;

class QmlEditFragment;
class QmlJsHighlighter;
class QmlJsSettings;
class QmlCompletionContextFinder;
class QmlCompletionContext;

class LV_EDITQMLJS_EXPORT CodeQmlHandler : public AbstractCodeHandler{

    Q_OBJECT
    Q_DISABLE_COPY(CodeQmlHandler)

public:
    explicit CodeQmlHandler(
        ViewEngine* engine,
        Project* project,
        QmlJsSettings *settings,
        ProjectQmlExtension* projectHandler,
        DocumentHandler* handler = 0
    );
    ~CodeQmlHandler();

    void assistCompletion(
        const QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        CodeCompletionModel* model,
        QTextCursor& cursorChange
    ) Q_DECL_OVERRIDE;
    void setDocument(ProjectDocument* document) Q_DECL_OVERRIDE;
    AbstractCodeHandler::ContentsTrigger documentContentsChanged(int position, int charsRemoved, int charsAdded) Q_DECL_OVERRIDE;
    void rehighlightBlock(const QTextBlock& block) Q_DECL_OVERRIDE;
    QPair<int, int> contextBlock(int position) Q_DECL_OVERRIDE;

    QList<lv::QmlDeclaration::Ptr> getDeclarations(const QTextCursor& cursor);
    bool findDeclarationValue(int position, int length, int& valuePosition, int& valueEnd);
    QmlEditFragment* createInjectionChannel(QmlDeclaration::Ptr property,
        QObject* runtime,
        CodePalette *converter
    );

    bool addEditingFragment(QmlEditFragment *edit);
    void removeEditingFragment(QmlEditFragment* edit);
    void removeEditingFragmentPalette(QmlEditFragment* palette);

    QmlJsSettings* settings();

public slots:
    // Palette management

    lv::PaletteList *findPalettes(int position, bool unrepeated = false);
    lv::CodePalette* openPalette(lv::PaletteList* palette, int index, QObject* currentApp = 0);
    void removePalette(QObject* palette);
    int palettePosition(QObject* palette);
    void paletteValueChanged(QmlEditFragment *editFragment);

    void addPaletteBox(QObject* paletteBox);
    QObject* paletteBoxFor(lv::CodePalette* palette);
    QObject* paletteBoxAtPosition(int position);

    // Binding management

    lv::DocumentCursorInfo* cursorInfo(int position, int length);
    lv::CodePalette* openBinding(lv::PaletteList* paletteList, int index, QObject* currentApp = 0);
    void closeBinding(int position, int length);

    // Direct editing management

    lv::CodePalette *edit(int position, QObject* currentApp = 0);
    void cancelEdit();

    // Add Property Management

    lv::QmlAddContainer* getAddOptions(int position);
    void addProperty(int position, const QString& addText);
    void addItem(int position, const QString& text);
    void updateRuntimeBindings(QObject* obj);

    // Scopes

    void newDocumentScopeReady(const QString& path, DocumentQmlScope::Ptr documentScope);
    void newProjectScopeReady();
    void updateScope();

signals:
    void paletteAboutToRemove(lv::CodePalette* palette);

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
    QTimer                 m_timer;

    QList<QObject*>               m_paletteBoxes;
    QLinkedList<QmlEditFragment*> m_edits; // opened palettes
    QmlEditFragment*              m_editingFragment; // editing fragment

};

inline QmlJsSettings *CodeQmlHandler::settings(){
    return m_settings;
}

}// namespace

#endif // LVCODEQMLHANDLER_H
