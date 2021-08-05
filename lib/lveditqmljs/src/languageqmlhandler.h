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

#ifndef LVLANGUAGEQMLHANDLER_H
#define LVLANGUAGEQMLHANDLER_H

#include "live/lveditqmljsglobal.h"
#include "live/projectdocument.h"
#include "live/projectqmlscope.h"
#include "live/qmldeclaration.h"
#include "live/lockedfileiosession.h"
#include "live/viewengine.h"
#include "live/settings.h"
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
class QmlMetaTypeInfo;

class LanguageQmlHandlerPrivate;
class LV_EDITQMLJS_EXPORT LanguageQmlHandler : public QObject{

    Q_OBJECT
    Q_DISABLE_COPY(LanguageQmlHandler)
    Q_PROPERTY(lv::CodeHandler*                 code            READ code                   CONSTANT)
    Q_PROPERTY(lv::QmlEditFragmentContainer*    editContainer   READ editContainer          CONSTANT)
    Q_PROPERTY(lv::DocumentQmlChannels*         bindingChannels READ bindingChannels        CONSTANT)
    Q_PROPERTY(QmlEditFragment*                 importsFragment READ importsFragment  WRITE setImportsFragment NOTIFY importsFragmentChanged)
    Q_PROPERTY(QmlEditFragment*                 rootFragment    READ rootFragment     WRITE setRootFragment    NOTIFY rootFragmentChanged)
public:
    friend class ProjectQmlExtension;
    friend class QmlEditFragmentContainer;

public:
    explicit LanguageQmlHandler(
        ViewEngine* engine,
        Project* project,
        QmlJsSettings *settings,
        ProjectQmlExtension* projectHandler,
        ProjectDocument *document,
        CodeHandler* handler = nullptr
    );
    ~LanguageQmlHandler() override;

    void assistCompletion(
        QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        CodeCompletionModel* model,
        QTextCursor& cursorChange
    );
    ProjectDocument* document() const;
    void rehighlightBlock(const QTextBlock& block);

    QmlDeclaration::Ptr getDeclarationViaCompletionContext(int position);
    QList<lv::QmlDeclaration::Ptr> getDeclarationsViaParsedDocument(int position, int length = 0);
    QList<lv::QmlDeclaration::Ptr> getDeclarations(int position, int length = 0);

    QmlEditFragment* createInjectionChannel(QmlDeclaration::Ptr property, QmlEditFragment* parent = nullptr);

    bool addEditFragment(QmlEditFragment *edit);
    void removeEditFragment(QmlEditFragment* edit);

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

    QmlEditFragmentContainer *editContainer();
    DocumentQmlChannels* bindingChannels() const;
    CodeHandler* code() const;

    QmlEditFragment *importsFragment() const;
    void setImportsFragment(QmlEditFragment *importsFragment);

    QmlEditFragment *rootFragment() const;
    void setRootFragment(QmlEditFragment *rootFragment);

    QmlInheritanceInfo inheritanceInfo(const QString& name);

signals:
    void importsFragmentChanged();
    void rootFragmentChanged();
public slots:
    //TOCHECK IF THESE ARE SLOTS
    bool areImportsScanned();

    lv::QmlEditFragment* findObjectFragmentByPosition(int position);
    lv::QmlEditFragment* findFragmentByPosition(int position);


public slots:
    void onDocumentParsed(QJSValue callback);
    void onImportsScanned(QJSValue callback);
    void removeSyncImportsListeners();

    // Help

    QList<int> languageFeatures() const;
    QString help(int position);
    void toggleComment(int position, int length);

    // Edit Fragments

    lv::QmlEditFragment* openConnection(int position);
    lv::QmlEditFragment* openNestedConnection(lv::QmlEditFragment* edit, int position);
    lv::QmlEditFragment* openReadOnlyPropertyConnection(lv::QmlEditFragment* parentFragment, QString name);
    QList<QObject*> openNestedFragments(lv::QmlEditFragment* edit, const QJSValue& options = QJSValue());
    QJSValue editFragments();

    void removeConnection(lv::QmlEditFragment* edit);
    void eraseObject(lv::QmlEditFragment* edit, bool removeFragment = true);

    // Palettes

    //TOMOVE: In findPalettesFromFragment
    QString defaultPalette(lv::QmlEditFragment* fragment);

    QJSValue findPalettesForFragment(lv::QmlEditFragment* fragment);
    QJSValue findPalettes(int position);
    lv::QmlEditFragment* removePalette(lv::CodePalette* palette);
    lv::CodePalette* openBinding(lv::QmlEditFragment* edit, QString paletteName);
    void closeBinding(int position, int length);
    QJSValue expand(lv::QmlEditFragment* edit, const QJSValue& val);

    // Document information

    QJSValue declarationInfo(int position, int length);
    QJSValue contextBlockRange(int cursorPosition);
    QJSValue getDocumentIds();
    int checkPragma(int position);

    lv::QmlMetaTypeInfo* typeInfo(const QString& fullTypeName);

    lv::QmlImportsModel* importsModel();

    int findImportsPosition();
    int findRootPosition();

    // Code Completion

    void suggestCompletion(int cursorPosition);

    // Direct editing management

    lv::CodePalette *edit(lv::QmlEditFragment* ef);
    void cancelEdit();

    // Add Property Management

    lv::QmlAddContainer* getAddOptions(QJSValue value);
    int addPropertyToCode(int position, const QString& name, const QString& value, lv::QmlEditFragment* parentGroup = nullptr);
    int addObjectToCode(int position, const QString& type, const QJSValue& properties = QJSValue());
    int addEventToCode(int position, const QString &name);
    int addRootObjectToCode(const QString &ctype);

    void createObjectForProperty(lv::QmlEditFragment* propertyFragment);
    void createObjectInRuntime(lv::QmlEditFragment* edit, const QString& type = "", const QJSValue& properties = QJSValue());

    // Registered slots

    void __whenLibraryScanQueueCleared();
    void __updateScope();

    void __documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void __documentFormatUpdate(int position, int length);
    void __cursorWritePositionChanged(QTextCursor cursor);

    void __newProjectScopeReady();
    void __aboutToDelete();

private:
    void setDocument(ProjectDocument* document);
    void createObjectInRuntimeImpl(lv::QmlEditFragment* edit, const QString& type = "", const QJSValue& properties = QJSValue());

    lv::QmlAddContainer* getAddOptionsForFragment(QmlEditFragment* edit, bool isReadOnly = false);
    lv::QmlAddContainer* getAddOptionsForPosition(int position);

    QmlDeclaration::Ptr createImportDeclaration();

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

    QJSValue findPalettesForDeclaration(QmlDeclaration::Ptr decl);

    void createChannelForFragment(QmlEditFragment* parentFragment, QmlEditFragment* fragment, QmlBindingPath::Ptr bindingPath);
    QJSValue declarationToQml(QmlDeclaration::Ptr decl);

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

    QmlEditFragment*              m_editFragment; // single edit fragment
    QmlEditFragmentContainer*     m_editContainer;

    DocumentQmlChannels*   m_bindingChannels;
    QmlEditFragment*       m_importsFragment;
    QmlEditFragment*       m_rootFragment;

    QScopedPointer<LanguageQmlHandlerPrivate> d_ptr;

    Q_DECLARE_PRIVATE(LanguageQmlHandler)
};

inline ProjectDocument *LanguageQmlHandler::document() const{
    return m_document;
}

/// \brief Returns the settings associated with this object.
inline QmlJsSettings *LanguageQmlHandler::settings(){
    return m_settings;
}

inline QmlEditFragmentContainer *LanguageQmlHandler::editContainer(){
    return m_editContainer;
}

inline DocumentQmlChannels *LanguageQmlHandler::bindingChannels() const{
    return m_bindingChannels;
}

}// namespace

#endif // LVLANGUAGEQMLHANDLER_H
