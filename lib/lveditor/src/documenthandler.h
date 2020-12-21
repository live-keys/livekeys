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

#ifndef LVDOCUMENTHANDLER_H
#define LVDOCUMENTHANDLER_H

#include <QObject>
#include <QQuickItem>
#include <QQmlParserStatus>
#include <QTextCursor>
#include <QJSValue>

#include "live/projectdocument.h"
#include "live/lveditorglobal.h"
#include "live/codecompletionmodel.h"

namespace lv{

class TextEdit;
class ViewEngine;
class Extensions;
class Project;

class LV_EDITOR_EXPORT DocumentHandler : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::CodeCompletionModel* completionModel READ completionModel CONSTANT)
    Q_PROPERTY(QObject*                 codeHandler     READ codeHandler     NOTIFY codeHandlerChanged)
    Q_PROPERTY(bool editorFocus                         READ editorFocus     WRITE setEditorFocus NOTIFY editorFocusChanged)
    Q_ENUMS(LanguageFeatures)

public:
    enum LanguageFeatures{
        LanguageHelp = 0,
        LanguageCodeCompletion,
        LanguageScope,
        LanguageHighlighting,
        LanguageJumpToDef,
        LanguageFindReferences,
        LanguageDiagnostics,
        LanguageLayout
    };

public:
    /** Paragraph separator char */
    static const QChar ParagraphSeparator;
    /** New line char */
    static const QChar NewLine;

public:
    explicit DocumentHandler(QObject* parent = nullptr);
    ~DocumentHandler();

    QTextDocument *target();
    void setTarget(QTextDocument *target);

    /**
     * \brief Completion model
     *
     * Model of suggestions that we get from the code handler
     */
    CodeCompletionModel* completionModel() const;

    void setIndentSize(int size);

    void classBegin(){}
    void componentComplete();

    /** Returns the TextEdit */
    TextEdit* textEdit();
    void setTextEdit(TextEdit* te);

    int currentCursorPosition() const;

    const QChar& lastAddedChar() const;

    /**
     * \brief Shows if the editor is in focus
     */
    bool editorFocus() const;
    void setEditorFocus(bool editorFocus);

    /**
     * \brief Code handler getter
     */
    QObject *codeHandler();

    void requestCursorPosition(int position);

    void lineBoxAdded(int lineStart, int lineEnd, int height, QQuickItem *palette, int start = -1, int end = -1);

public slots:
    void insertCompletion(int from, int to, const QString& completion);
    void documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void setDocument(lv::ProjectDocument* document, QJSValue options = QJSValue());

    void manageIndent(int from, int length, bool undo = false);
    void insertTab(int position);

    bool has(int feature);

signals:
    /** Target changed */
    void targetChanged();
    /** Cursor position change request */
    void cursorPositionRequest(int position);
    /** Document content was changed manually */
    void contentsChangedManually();
    /** Editor focus changed */
    void editorFocusChanged();
    /** Code handler changed */
    void codeHandlerChanged();
    /** Triggered just before the handler is to be destroyed */
    void aboutToDeleteHandler();

private:
    void findCodeHandler();
    void updateCodeHandlerTarget();

    QChar                 m_lastChar;
    QTextDocument*        m_targetDoc;
    CodeCompletionModel*  m_completionModel;
    QObject*              m_codeHandler;
    ProjectDocument*      m_projectDocument;
    int                   m_indentSize;
    QString               m_indentContent;
    Project*              m_project;
    Extensions*           m_extensions;
    ViewEngine*           m_engine;
    TextEdit*             m_textEdit;

    QSet<int>             m_languageFeatures;

    bool                  m_editorFocus;
};

/**
 * \brief Returns the target text document
 */
inline QTextDocument *DocumentHandler::target(){
    return m_targetDoc;
}

inline lv::CodeCompletionModel *DocumentHandler::completionModel() const{
    return m_completionModel;
}

/** \brief Returns the TextEdit */
inline TextEdit *DocumentHandler::textEdit(){
    return m_textEdit;
}

/** \brief Returns true if the current editor has focus */
inline bool DocumentHandler::editorFocus() const{
    return m_editorFocus;
}

/**
 * \brief Sets the focus state of the editor
 */
inline void DocumentHandler::setEditorFocus(bool editorFocus){
    if ( m_editorFocus == editorFocus )
        return;
    m_editorFocus = editorFocus;
    emit editorFocusChanged();
}

inline QObject *DocumentHandler::codeHandler(){
    return m_codeHandler;
}

/**
 * \brief Indent size setter
 */
inline void DocumentHandler::setIndentSize(int size){
    m_indentContent.clear();
    m_indentContent.fill(QChar(' '), size);
    m_indentSize = size;
}

}// namespace

#endif // LVDOCUMENTHANDLER_H
