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

#ifndef LVDOCUMENTHANDLER_H
#define LVDOCUMENTHANDLER_H

#include <QObject>
#include <QQmlParserStatus>
#include <QJSValue>
#include <QTimer>
#include <QTextCursor>

#include "live/projectdocument.h"
#include "live/lveditorglobal.h"
#include "live/codecompletionmodel.h"
#include "live/documentcursorinfo.h"
#include "live/abstractcodehandler.h"

namespace lv{

class TextEdit;
class Project;
class ViewEngine;

class LivePalette;
class LivePaletteList;
class LivePaletteContainer;

class DocumentHandlerState;
class LV_EDITOR_EXPORT DocumentHandler : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::CodeCompletionModel* completionModel READ completionModel CONSTANT)
    Q_PROPERTY(bool editorFocus                         READ editorFocus     WRITE setEditorFocus NOTIFY editorFocusChanged)

public:
    static const QChar ParagraphSeparator;
    static const QChar NewLine;

public:
    explicit DocumentHandler(QObject* parent = 0);
    ~DocumentHandler();

    QTextDocument *target();
    void setTarget(QTextDocument *target);

    CodeCompletionModel* completionModel() const;

    void rehighlightBlock(const QTextBlock& block);

    DocumentHandlerState* state();

    void setIndentSize(int size);

    void classBegin(){}
    void componentComplete();

    bool addEditingPalette(DocumentEditFragment *palette);
    void removeEditingPalette(DocumentEditFragment* palette);

    TextEdit* textEdit();
    void setTextEdit(TextEdit* te);

    bool editorFocus() const;
    void setEditorFocus(bool editorFocus);

public slots:
    void insertCompletion(int from, int to, const QString& completion);
    void documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void cursorWritePositionChanged(QTextCursor cursor);
    void setDocument(lv::ProjectDocument* document, QJSValue options = QJSValue());
    void documentUpdatedContent();
    void generateCompletion(int cursorPosition);
    void updateScope();
    void bind(int position, int length, QObject* object = 0);
    void unbind(int position, int length);
    lv::LivePaletteList *findPalettes(int position);
    void openPalette(lv::LivePalette* palette, int position, QObject* currentApp = 0);
    void removePalette(QObject* palette);
    void manageIndent(int from, int length, bool undo = false);

    lv::DocumentCursorInfo* cursorInfo(int position, int length);
    QJSValue contextBlockRange(int cursorPosition);

    bool edit(int position, QObject* currentApp = 0);
    void commitEdit();
    void cancelEdit();

    void paletteValueChanged(DocumentEditFragment *editFragment);

signals:
    void targetChanged();
    void cursorPositionRequest(int position);
    void contentsChangedManually();
    void paletteAboutToRemove(lv::LivePalette* palette);
    void fragmentLinesChanged(int lineStart, int lineEnd);
    void editingStateChanged(bool isEditing);
    void editorFocusChanged();

private:
    void readContent();
    void updateFragments();
    void findCodeHandler();
    void updateCodeHandlerTarget();
    void rehighlightSection(int position, int length);

    QChar                 m_lastChar;
    QTextDocument*        m_targetDoc;
    CodeCompletionModel*  m_completionModel;
    AbstractCodeHandler*  m_codeHandler;
    ProjectDocument*      m_projectDocument;
    int                   m_indentSize;
    QString               m_indentContent;
    LivePaletteContainer* m_paletteContainer;
    Project*              m_project;
    ViewEngine*               m_engine;
    QTimer                m_timer;
    TextEdit*             m_textEdit;

    ProjectDocumentMarker::Ptr m_fragmentStart;
    ProjectDocumentMarker::Ptr m_fragmentEnd;
    int m_fragmentStartLine;
    int m_fragmentEndLine;

    DocumentHandlerState* m_state;

    QLinkedList<DocumentEditFragment*> m_palettes; // opened palettes
    DocumentEditFragment*              m_editingFragment; // editing fragment
    bool m_editorFocus;
};

inline QTextDocument *DocumentHandler::target(){
    return m_targetDoc;
}

inline lv::CodeCompletionModel *DocumentHandler::completionModel() const{
    return m_completionModel;
}

inline DocumentHandlerState *DocumentHandler::state(){
    return m_state;
}

inline TextEdit *DocumentHandler::textEdit(){
    return m_textEdit;
}

inline bool DocumentHandler::editorFocus() const{
    return m_editorFocus;
}

inline void DocumentHandler::setEditorFocus(bool editorFocus){
    if ( m_editorFocus == editorFocus )
        return;
    m_editorFocus = editorFocus;
    emit editorFocusChanged();
}

inline void DocumentHandler::setIndentSize(int size){
    m_indentContent.clear();
    m_indentContent.fill(QChar(' '), size);
    m_indentSize = size;
}

}// namespace

#endif // LVDOCUMENTHANDLER_H
