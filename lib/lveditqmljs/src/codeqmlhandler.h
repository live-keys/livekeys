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
#include "qmlscopesnap_p.h"

#include <QObject>
#include <QTimer>
#include <QTextCursor>
#include <QQmlListProperty>

class QQmlEngine;

namespace lv{

class ProjectQmlScanner;
class ProjectQmlExtension;

class DocumentQmlChannels;
class QmlEditFragment;
class QmlEditFragmentContainer;
class QmlJsHighlighter;
class QmlJsSettings;
class QmlAddContainer;
class QmlCompletionContextFinder;
class QmlCompletionContext;
class QmlUsageGraphScanner;
class QmlSuggestionModel;

class CodeQmlHandlerPrivate;
class LV_EDITQMLJS_EXPORT CodeQmlHandler : public QObject{

    Q_OBJECT
    Q_DISABLE_COPY(CodeQmlHandler)
    Q_PROPERTY(lv::QmlEditFragmentContainer* editContainer READ editContainer   CONSTANT)
    Q_PROPERTY(lv::DocumentQmlChannels* bindingChannels    READ bindingChannels CONSTANT)

public:
    friend class ProjectQmlExtension;
    friend class QmlEditFragmentContainer;

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
    ProjectDocument* document() const;
    void rehighlightBlock(const QTextBlock& block);

    QmlDeclaration::Ptr getDeclarationViaCompletionContext(int position);
    QList<lv::QmlDeclaration::Ptr> getDeclarationsViaParsedDocument(int position, int length = 0);
    QList<lv::QmlDeclaration::Ptr> getDeclarations(int position, int length = 0);

    QmlEditFragment* createInjectionChannel(QmlDeclaration::Ptr property, QmlEditFragment* parent = nullptr);

    bool addEditingFragment(QmlEditFragment *edit);
    void removeEditingFragment(QmlEditFragment* edit);

    QmlJsSettings* settings();

    void suggestionsForProposedExpression(
        QmlDeclaration::Ptr declaration,
        const QString& expression,
        CodeCompletionModel* model,
        bool suggestFunctions = false) const;
    bool findBindingForExpression(QmlEditFragment* edit, const QString& expression);
    bool findFunctionBindingForExpression(QmlEditFragment* edit, const QString& expression);

    QmlUsageGraphScanner* createScanner();

    void newDocumentScanReady(DocumentQmlInfo::Ptr documentInfo);

    enum AddOptionsFilter {
        ReadOnly = 1,
        ForNode = 2,
        GroupOnly = 4,
        NoReadOnly = 8
    };

    QmlEditFragmentContainer *editContainer();
    DocumentQmlChannels* bindingChannels() const;

    Q_ENUMS(AddOptionsFilter);
public slots:
    void __whenLibraryScanQueueCleared();
    bool areImportsScanned();

    void onDocumentParse(QJSValue callback);
    void onImportsScanned(QJSValue callback);
    void removeSyncImportsListeners();

    QList<int> languageFeatures() const;

    void populateNestedObjectsForFragment(lv::QmlEditFragment* ef);
    void populateObjectInfoForFragment(lv::QmlEditFragment* ef);
    void populatePropertyInfoForFragment(lv::QmlEditFragment* ef);
    QVariantMap propertiesWritable(lv::QmlEditFragment* ef);

    // Help

    QString help(int position);

    lv::QmlEditFragment* findObjectFragmentByPosition(int position);
    lv::QmlEditFragment* findFragmentByPosition(int position);
    QJSValue editingFragments();

    lv::QmlEditFragment* findChildPropertyFragmentByName(lv::QmlEditFragment* parent, QString name) const;

    void toggleComment(int position, int length);

    // Palette and binding management

    QJSValue cursorInfo(int position, int length);

    lv::QmlEditFragment* openConnection(int position);
    lv::QmlEditFragment* openNestedConnection(lv::QmlEditFragment* edit, int position);
    lv::QmlEditFragment* createReadOnlyFragment(lv::QmlEditFragment* parentFragment, QString name);
    QList<QObject*> openNestedObjects(lv::QmlEditFragment* edit);
    QList<QObject*> openNestedProperties(lv::QmlEditFragment* edit);
    void removeConnection(lv::QmlEditFragment* edit);
    void eraseObject(lv::QmlEditFragment* edit);

    QString propertyType(lv::QmlEditFragment* edit, const QString& propertyName);

    lv::PaletteList *findPalettesFromFragment(lv::QmlEditFragment* fragment, bool includeLayoutConfigurations = false);
    lv::PaletteList *findPalettes(int position, bool includeLayoutConfigurations = false);

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
    void addLineAtIndex(QString line, int pos);
    void removeLineAtIndex(int pos);

    int findImportsPosition();
    int findRootPosition();

    // Direct editing management

    lv::CodePalette *edit(lv::QmlEditFragment* ef);
    void cancelEdit();

    // Add Property Management

    lv::QmlAddContainer* getAddOptions(int position, int filter = 0, lv::QmlEditFragment* fragment = nullptr);
    int addProperty(
        int position,
        const QString& object,
        const QString& type,
        const QString& name,
        bool assignDefault = false,
        lv::QmlEditFragment* parentGroup = nullptr);
    int addItem(int position, const QString& object, const QString& type);
    int insertRootItem(const QString &ctype);
    int addEvent(int position, const QString &object, const QString &type, const QString &name);
    void addItemToRuntime(lv::QmlEditFragment* edit, const QString& type, QObject* currentApp = nullptr);

    lv::QmlEditFragment* createObject(int position, const QString& type, lv::QmlEditFragment* parent, QObject* currentApp = nullptr);

    QJSValue getDocumentIds();

    // Scopes

    void updateScope();

    QVariantList nestedObjectsInfo(lv::QmlEditFragment* ef);

    void suggestCompletion(int cursorPosition);

    // Registered slots

    void __documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void __documentFormatUpdate(int position, int length);
    void __cursorWritePositionChanged(QTextCursor cursor);

    void __newProjectScopeReady();
    void __aboutToDelete();

private:
    QmlDeclaration::Ptr createImportDeclaration();
    QJSValue createCursorInfo(bool canBind, bool canUnbind, bool canEdit, bool canAdjust, bool canShape, bool inImports = false);

    void rehighlightSection(int start, int end);
    void resetProjectQmlExtension();
    QString getHelpEntity(int position);

    void addPropertiesAndFunctionsToModel(const QmlInheritanceInfo& typePath, lv::QmlSuggestionModel* model, bool isForNode = false);
    void addObjectsToModel(const QmlScopeSnap& scope, lv::QmlSuggestionModel* model);

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

    lv::PaletteList* findPalettesForDeclaration(QmlDeclaration::Ptr decl, bool includeExpandables = false);

private:
    QTextDocument*      m_target;
    QmlJsHighlighter*   m_highlighter;
    QmlJsSettings*      m_settings;
    ViewEngine*         m_engine;
    QmlCompletionContextFinder* m_completionContextFinder;

    ProjectDocument*       m_document;

    QLinkedList<QJSValue>  m_documentParseListeners;
    QLinkedList<QJSValue>  m_importsScannedListeners;

    bool                   m_newScope;
    QTimer                 m_scopeTimer;

    QmlEditFragment*              m_editingFragment; // single editing fragment
    QmlEditFragmentContainer*     m_editContainer;

    DocumentQmlChannels*   m_bindingChannels;

    QScopedPointer<CodeQmlHandlerPrivate> d_ptr;

    Q_DECLARE_PRIVATE(CodeQmlHandler)
};

inline ProjectDocument *CodeQmlHandler::document() const{
    return m_document;
}

/// \brief Returns the settings associated with this object.
inline QmlJsSettings *CodeQmlHandler::settings(){
    return m_settings;
}

inline QmlEditFragmentContainer *CodeQmlHandler::editContainer(){
    return m_editContainer;
}

inline DocumentQmlChannels *CodeQmlHandler::bindingChannels() const{
    return m_bindingChannels;
}

}// namespace

#endif // LVCODEQMLHANDLER_H
