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
#include <QQuickItem>
#include <QQmlParserStatus>
#include <QTextCursor>
#include <QJSValue>

#include "live/projectdocument.h"
#include "live/lveditorglobal.h"
#include "live/codecompletionmodel.h"
#include "live/abstractcodehandler.h"

namespace lv{

class TextEdit;
class ViewEngine;
class Extensions;
class Project;

class LV_EDITOR_EXPORT DocumentHandler : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(lv::CodeCompletionModel* completionModel READ completionModel CONSTANT)
    Q_PROPERTY(lv::AbstractCodeHandler* codeHandler     READ codeHandler     NOTIFY codeHandlerChanged)
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
    void rehighlightSection(int position, int length);

    void setIndentSize(int size);

    void classBegin(){}
    void componentComplete();

    TextEdit* textEdit();
    void setTextEdit(TextEdit* te);

    bool editorFocus() const;
    void setEditorFocus(bool editorFocus);

    AbstractCodeHandler* codeHandler();

    void requestCursorPosition(int position);

    void lineBoxAdded(int lineStart, int lineEnd, int height, QQuickItem *palette);
    void lineBoxRemoved(QQuickItem *palette);
public slots:
    void insertCompletion(int from, int to, const QString& completion);
    void documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void cursorWritePositionChanged(QTextCursor cursor);
    void setDocument(lv::ProjectDocument* document, QJSValue options = QJSValue());
    void documentFormatUpdate(int position, int length);
    void generateCompletion(int cursorPosition);
    QJSValue contextBlockRange(int cursorPosition);

    void manageIndent(int from, int length, bool undo = false);

signals:
    void targetChanged();
    void cursorPositionRequest(int position);
    void contentsChangedManually();
    void editorFocusChanged();
    void codeHandlerChanged();

private:
    void readContent();
    void findCodeHandler();
    void updateCodeHandlerTarget();

    QChar                 m_lastChar;
    QTextDocument*        m_targetDoc;
    CodeCompletionModel*  m_completionModel;
    AbstractCodeHandler*  m_codeHandler;
    ProjectDocument*      m_projectDocument;
    int                   m_indentSize;
    QString               m_indentContent;
    Project*              m_project;
    Extensions*           m_extensions;
    ViewEngine*           m_engine;
    TextEdit*             m_textEdit;

    bool                  m_editorFocus;
};

inline QTextDocument *DocumentHandler::target(){
    return m_targetDoc;
}

inline lv::CodeCompletionModel *DocumentHandler::completionModel() const{
    return m_completionModel;
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

inline AbstractCodeHandler *DocumentHandler::codeHandler(){
    return m_codeHandler;
}

inline void DocumentHandler::setIndentSize(int size){
    m_indentContent.clear();
    m_indentContent.fill(QChar(' '), size);
    m_indentSize = size;
}

}// namespace

#endif // LVDOCUMENTHANDLER_H
