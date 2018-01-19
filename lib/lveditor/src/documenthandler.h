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

class QQuickTextDocument;

namespace lv{

class Project;
class Engine;

class LivePalette;
class LivePaletteContainer;

class DocumentHandlerState;
class LV_EDITOR_EXPORT DocumentHandler : public QObject, public QQmlParserStatus{

    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQuickTextDocument* target               READ target          WRITE setTarget  NOTIFY targetChanged)
    Q_PROPERTY(lv::LivePalette* palette                 READ palette         NOTIFY paletteChanged)
    Q_PROPERTY(lv::CodeCompletionModel* completionModel READ completionModel CONSTANT)

public:
    enum EditingState{
        Manual   = 0, //     0 : coming from the user
        Assisted = 1, //     1 : coming from a code completion assistant
        Silent   = 2, //    10 : does not trigger a recompile
        Palette  = 6, //   110 : also silent (when a palette edits a section)
        Runtime  = 10,//  1010 : also silent (comming from a runtime binding)
        Read     = 16 // 10000 : populate from project document, does not signal anything
    };

    static const QChar ParagraphSeparator;
    static const QChar NewLine;

public:
    explicit DocumentHandler(QObject* parent = 0);
    ~DocumentHandler();

    QQuickTextDocument *target();
    void setTarget(QQuickTextDocument *target);

    CodeCompletionModel* completionModel() const;

    LivePalette* palette();

    void addEditingState(EditingState type);
    void removeEditingState(EditingState state);
    bool editingStateIs(int flag);
    void resetEditingState();

    void rehighlightBlock(const QTextBlock& block);

    DocumentHandlerState* state();

    void setIndentSize(int size);

    void classBegin(){}
    void componentComplete();

public slots:
    void insertCompletion(int from, int to, const QString& completion);
    void documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void cursorWritePositionChanged(QTextCursor cursor);
    void setDocument(lv::ProjectDocument* document, QJSValue options = QJSValue());
    void documentUpdatedContent(QObject* author);
    void generateCompletion(int cursorPosition);
    void updateScope();
    void bind(int position, int length, QObject* object = 0);
    void unbind(int position, int length);
    void edit(int position, QObject* currentApp = 0);
    void adjust(int position, QObject* currentApp = 0);
    void manageIndent(int from, int length, bool undo = false);

    lv::DocumentCursorInfo* cursorInfo(int position, int length);

    void commitEdit();
    void cancelEdit();
    bool isEditing();

    void paletteValueChanged();

signals:
    void targetChanged();
    void cursorPositionRequest(int position);
    void contentsChangedManually();
    void paletteChanged();
    void fragmentLinesChanged(int lineStart, int lineEnd);

private:
    void readContent();
    void updateFragments();
    void findCodeHandler();
    void updateCodeHandlerTarget();
    void rehighlightSection(int position, int length);

    QChar                 m_lastChar;
    QQuickTextDocument*   m_target;
    QTextDocument*        m_targetDoc;
    CodeCompletionModel*  m_completionModel;
    AbstractCodeHandler*  m_codeHandler;
    ProjectDocument*      m_projectDocument;
    int                   m_editingState;
    int                   m_indentSize;
    QString               m_indentContent;
    LivePaletteContainer* m_paletteContainer;
    Project*              m_project;
    Engine*               m_engine;
    QTimer                m_timer;

    ProjectDocumentMarker::Ptr m_fragmentStart;
    ProjectDocumentMarker::Ptr m_fragmentEnd;
    int m_fragmentStartLine;
    int m_fragmentEndLine;

    DocumentHandlerState* m_state;
};

inline QQuickTextDocument *DocumentHandler::target(){
    return m_target;
}

inline lv::CodeCompletionModel *DocumentHandler::completionModel() const{
    return m_completionModel;
}

inline void DocumentHandler::addEditingState(EditingState state){
    m_editingState |= state;
}

inline void DocumentHandler::removeEditingState(EditingState state){
    if ( m_editingState & state ){
        bool restoreSilent = editingStateIs(DocumentHandler::Palette | DocumentHandler::Runtime);
        m_editingState = m_editingState & ~state;
        if ( restoreSilent ){
            m_editingState |= DocumentHandler::Silent;
        }
    }
}

inline bool DocumentHandler::editingStateIs(int flag){
    return (flag & m_editingState) == flag;
}

inline void DocumentHandler::resetEditingState(){
    m_editingState = 0;
}

inline DocumentHandlerState *DocumentHandler::state(){
    return m_state;
}

inline void DocumentHandler::setIndentSize(int size){
    m_indentContent.clear();
    m_indentContent.fill(QChar(' '), size);
    m_indentSize = size;
}

}// namespace

#endif // LVDOCUMENTHANDLER_H
