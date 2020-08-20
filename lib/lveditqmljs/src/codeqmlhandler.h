/****************************************************************************
**
** Copyright (C) 2014-2019 Dinu SV.
** (contact: mail@dinusv.com)
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

#ifndef LVCODEQMLHANDLER_H
#define LVCODEQMLHANDLER_H

#include "live/lveditqmljsglobal.h"
#include "live/projectdocument.h"
#include "live/projectqmlscope.h"
#include "live/qmldeclaration.h"
#include "live/lockedfileiosession.h"
#include "live/viewengine.h"
#include "live/settings.h"
#include "live/palettelist.h"
#include "live/codecompletionmodel.h"
#include "live/qmlimportsmodel.h"
#include "live/documentqmlinfo.h"

#include <QObject>
#include <QTimer>
#include <QTextCursor>
#include <QQmlListProperty>

class QQmlEngine;

namespace lv{

class ProjectQmlScanner;
class ProjectQmlExtension;

class QmlEditFragment;
class QmlJsHighlighter;
class QmlJsSettings;
class QmlAddContainer;
class QmlCompletionContextFinder;
class QmlCompletionContext;
class QmlUsageGraphScanner;

class CodeQmlHandlerPrivate;
class LV_EDITQMLJS_EXPORT CodeQmlHandler : public QObject{

    Q_OBJECT
    Q_DISABLE_COPY(CodeQmlHandler)

    friend class ProjectQmlExtension;

    Q_PROPERTY(int numberOfConnections READ numberOfConnections NOTIFY numberOfConnectionsChanged)

public:
    explicit CodeQmlHandler(
        ViewEngine* engine,
        Project* project,
        QmlJsSettings *settings,
        ProjectQmlExtension* projectHandler,
        ProjectDocument *document,
        DocumentHandler* handler = nullptr
    );
    ~CodeQmlHandler() override;

    void assistCompletion(
        QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        CodeCompletionModel* model,
        QTextCursor& cursorChange
    );
    void setDocument(ProjectDocument* document);
    void rehighlightBlock(const QTextBlock& block);

    QList<lv::QmlDeclaration::Ptr> getDeclarations(const QTextCursor& cursor);
    bool findDeclarationValue(int position, int length, int& valuePosition, int& valueEnd);
    QmlEditFragment* createInjectionChannel(QmlDeclaration::Ptr property);

    bool addEditingFragment(QmlEditFragment *edit);
    void removeEditingFragment(QmlEditFragment* edit);

    QmlJsSettings* settings();

    QmlEditFragment* findEditFragment(CodePalette* palette);
    QmlEditFragment* findEditFragmentIn(QmlEditFragment *parent, CodePalette* palette);

    void suggestionsForProposedExpression(
        QmlDeclaration::Ptr declaration,
        const QString& expression,
        CodeCompletionModel* model,
        bool suggestFunctions = false) const;
    bool findBindingForExpression(QmlEditFragment* edit, const QString& expression);
    bool findFunctionBindingForExpression(QmlEditFragment* edit, const QString& expression);

    QmlUsageGraphScanner* createScanner();

    int numberOfConnections();

    void newDocumentScanReady(DocumentQmlInfo::Ptr documentInfo);

public slots:
    void processingChanged(bool value);

    QList<int> languageFeatures() const;
    void populateNestedObjectsForFragment(lv::QmlEditFragment* ef);
    void populateObjectInfoForFragment(lv::QmlEditFragment* ef);
    void populatePropertyInfoForFragment(lv::QmlEditFragment* ef);
    void testFunction(QVariantList list);

    // Help

    QString help(int position);
    lv::QmlEditFragment* findObjectFragmentByPosition(int position);
    lv::QmlEditFragment* findFragmentByPosition(int position);
    QJSValue editingFragments();


    void toggleComment(int position, int length);

    // Palette and binding management

    QJSValue cursorInfo(int position, int length);
    bool isInImports(int position);
    lv::QmlEditFragment* openConnection(int position);
    lv::QmlEditFragment* openNestedConnection(lv::QmlEditFragment* edit, int position);
    QList<QObject*> openNestedObjects(lv::QmlEditFragment* edit);
    QList<QObject*> openNestedProperties(lv::QmlEditFragment* edit);
    void removeConnection(lv::QmlEditFragment* edit);
    void deleteObject(lv::QmlEditFragment* edit);

    QString propertyType(lv::QmlEditFragment* edit, const QString& propertyName);

    lv::PaletteList *findPalettes(int position, bool unrepeated = false, bool includeExpandables = false);

    QJSValue openPalette(lv::QmlEditFragment* fragment, lv::PaletteList* palette, int index);
    lv::QmlEditFragment* removePalette(lv::CodePalette* palette);

    QString defaultPalette(lv::QmlEditFragment* fragment);

    lv::CodePalette* openBinding(lv::QmlEditFragment* edit, lv::PaletteList* paletteList, int index);
    void closeBinding(int position, int length);

    QJSValue expand(lv::QmlEditFragment* edit, const QJSValue& val);

    bool isForAnObject(lv::QmlEditFragment* palette);

    void frameEdit(QQuickItem *box, lv::QmlEditFragment* palette);
    QJSValue contextBlockRange(int cursorPosition);

    lv::QmlImportsModel* importsModel();
    void addLineAtPosition(QString line, int pos);
    void removeLineAtPosition(int pos);
    void removeAllEditingFragments();

    int findImportsPosition(int blockPos);
    int findRootPosition();

    // Direct editing management

    lv::CodePalette *edit(lv::QmlEditFragment* ef);
    void cancelEdit();

    // Add Property Management

    lv::QmlAddContainer* getAddOptions(int position, bool includeFunctions = false);
    int addProperty(
        int position,
        const QString& object,
        const QString& type,
        const QString& name,
        bool assignDefault = false);
    int addItem(int position, const QString& object, const QString& type);
    int insertRootItem(const QString &ctype);
    int addEvent(int position, const QString &object, const QString &type, const QString &name);
    void addItemToRuntime(lv::QmlEditFragment* edit, const QString& type, QObject* currentApp = nullptr);
    void updateRuntimeBindings();

    lv::QmlEditFragment* createObject(int position, const QString& type, lv::QmlEditFragment* parent, QObject* currentApp = nullptr);

    QJSValue getDocumentIds();

    void suggestCompletion(int cursorPosition);

    void __documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void __documentFormatUpdate(int position, int length);
    void __cursorWritePositionChanged(QTextCursor cursor);

    // Scopes

    void newProjectScopeReady();
    void updateScope();

    void aboutToDelete();
    QVariantList nestedObjectsInfo(lv::QmlEditFragment* ef);
    QString getFragmentId(lv::QmlEditFragment* ef);
signals:
    void numberOfConnectionsChanged();
    void stoppedProcessing();
private:
    QJSValue createCursorInfo(bool canBind, bool canUnbind, bool canEdit, bool canAdjust, bool canShape, bool inImports = false);

    void rehighlightSection(int start, int end);
    void resetProjectQmlExtension();
    QString getHelpEntity(int position);

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

    void suggestionsForNamespaceTypes(
        const QString& typeNameSpace,
        QList<CodeCompletionSuggestion>& suggestions
    ) const;
    void suggestionsForNamespaceImports(
        QList<CodeCompletionSuggestion>& suggestions
    );
    void suggestionsForDocumentsIds(
        QList<CodeCompletionSuggestion>& suggestions
    ) const;

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

    QString getBlockIndent(const QTextBlock& bl);
    bool isBlockEmptySpace(const QTextBlock& bl);
    bool isForAnObject(const QmlDeclaration::Ptr& declaration);

    QTextDocument*      m_target;
    QmlJsHighlighter*   m_highlighter;
    QmlJsSettings*      m_settings;
    QQmlEngine*         m_engine;
    QmlCompletionContextFinder* m_completionContextFinder;

    ProjectDocument*       m_document;

    bool                   m_newScope;
    QTimer                 m_scopeTimer;

    QLinkedList<QmlEditFragment*> m_edits; // opened palettes
    QmlEditFragment*              m_editingFragment; // editing fragment

    QScopedPointer<CodeQmlHandlerPrivate> d_ptr;

    Q_DECLARE_PRIVATE(CodeQmlHandler)
};

/// \brief Returns the settings associated with this object.
inline QmlJsSettings *CodeQmlHandler::settings(){
    return m_settings;
}

}// namespace

#endif // LVCODEQMLHANDLER_H
