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

#ifndef QDOCUMENTHANDLER_H
#define QDOCUMENTHANDLER_H

#include "qlcveditorglobal.h"
#include "qcodecompletionmodel.h"
#include "qdocumentcursorinfo.h"
#include "qabstractcodehandler.h"

#include <QObject>
#include <QTimer>
#include <QTextCursor>

class QQuickTextDocument;

namespace lcv{

class QDocumentHandlerState;
class QLivePalette;
class QLivePaletteContainer;
class Q_LCVEDITOR_EXPORT QDocumentHandler : public QObject{

    Q_OBJECT
    Q_PROPERTY(QQuickTextDocument* target                 READ target          WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(lcv::QCodeCompletionModel* completionModel READ completionModel CONSTANT)
    Q_PROPERTY(lcv::QLivePalette* palette                 READ palette         NOTIFY paletteChanged)

public:
    enum EditingState{
        Manual   = 0, //    0 : coming from the user
        Assisted = 1, //    1 : coming from a code completion assistant
        Silent   = 2, //   10 : does not trigger a recompile
        Palette  = 6, //  110 : also silent (when a palette edits a section)
        Runtime  = 10,// 1010 : also silent (comming from a runtime binding)
    };

public:
    explicit QDocumentHandler(
        QAbstractCodeHandler* handler,
        QLivePaletteContainer* paletteContainer = 0,
        QObject* parent = 0
    );
    ~QDocumentHandler();

    QQuickTextDocument *target();
    void setTarget(QQuickTextDocument *target);

    lcv::QCodeCompletionModel* completionModel() const;

    QLivePalette* palette();

    void addEditingState(EditingState type);
    void removeEditingState(EditingState state);
    bool editingStateIs(int flag);
    void resetEditingState();

    void rehighlightBlock(const QTextBlock& block);

    QDocumentHandlerState* state();

    void setIndentSize(int size);

public slots:
    void insertCompletion(int from, int to, const QString& completion);
    void documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void cursorWritePositionChanged(QTextCursor cursor);
    void setDocument(QProjectDocument* document);
    void generateCompletion(int cursorPosition);
    void updateScope(const QString& data);
    void bind(int position, int length, QObject* object = 0);
    void unbind(int position, int length);
    void edit(int position, QObject* currentApp = 0);
    void adjust(int position, QObject* currentApp = 0);
    void manageIndent(int from, int length, bool undo = false);

    lcv::QDocumentCursorInfo* cursorInfo(int position, int length);

    void commitEdit();
    void cancelEdit();
    bool isEditing();

    void paletteValueChanged();

signals:
    void targetChanged();
    void cursorPositionRequest(int position);
    void contentsChangedManually();
    void paletteChanged();

private:
    void rehighlightSection(int position, int length);

    QChar                      m_lastChar;
    QQuickTextDocument*        m_target;
    QTextDocument*             m_targetDoc;
    QCodeCompletionModel*      m_completionModel;
    QAbstractCodeHandler*      m_codeHandler;
    QProjectDocument*          m_projectDocument;
    int                        m_editingState;
    int                        m_indentSize;
    QString                    m_indentContent;
    QLivePaletteContainer*     m_paletteContainer;

    QDocumentHandlerState*     m_state;
};

inline QQuickTextDocument *QDocumentHandler::target(){
    return m_target;
}

inline lcv::QCodeCompletionModel *QDocumentHandler::completionModel() const{
    return m_completionModel;
}

inline void QDocumentHandler::addEditingState(EditingState state){
    m_editingState |= state;
}

inline void QDocumentHandler::removeEditingState(EditingState state){
    if ( m_editingState & state ){
        bool restoreSilent = editingStateIs(QDocumentHandler::Palette | QDocumentHandler::Runtime);
        m_editingState = m_editingState & ~state;
        if ( restoreSilent ){
            m_editingState |= QDocumentHandler::Silent;
        }
    }
}

inline bool QDocumentHandler::editingStateIs(int flag){
    return (flag & m_editingState) == flag;
}

inline void QDocumentHandler::resetEditingState(){
    m_editingState = 0;
}

inline QDocumentHandlerState *QDocumentHandler::state(){
    return m_state;
}

inline void QDocumentHandler::setIndentSize(int size){
    m_indentContent.clear();
    m_indentContent.fill(QChar(' '), size);
    m_indentSize = size;
}

}// namespace

#endif // QDOCUMENTHANDLER_H
