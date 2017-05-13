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

#ifndef QDOCUMENTCODEINTERFACE_H
#define QDOCUMENTCODEINTERFACE_H

#include "qlcveditorglobal.h"
#include "qcodecompletionmodel.h"
#include "qabstractcodehandler.h"

#include <QObject>
#include <QTextCursor>

class QQuickTextDocument;

namespace lcv{

class QDocumentCodeState;
class QLivePalette;
class QLivePaletteContainer;
class Q_LCVEDITOR_EXPORT QDocumentCodeInterface : public QObject{

    Q_OBJECT
    Q_PROPERTY(QQuickTextDocument* target                 READ target          WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(lcv::QCodeCompletionModel* completionModel READ completionModel CONSTANT)
    Q_PROPERTY(lcv::QLivePalette* palette                 READ palette         NOTIFY paletteChanged)

public:
    explicit QDocumentCodeInterface(
        QAbstractCodeHandler* handler,
        QLivePaletteContainer* paletteContainer = 0,
        QObject* parent = 0
    );
    ~QDocumentCodeInterface();

    QQuickTextDocument *target();
    void setTarget(QQuickTextDocument *target);

    lcv::QCodeCompletionModel* completionModel() const;

    QLivePalette* palette();

    void enableSilentEditing();
    void disableSilentEditing();

    void rehighlightBlock(const QTextBlock& block);

    QDocumentCodeState* state();

public slots:
    void insertCompletion(int from, int to, const QString& completion);
    void documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void cursorWritePositionChanged(QTextCursor cursor);
    void setDocument(QProjectDocument* document);
    void generateCompletion(int cursorPosition);
    void updateScope(const QString& data);
    bool canBind(int position, int length);
    void bind(int position, int length, QObject* object = 0);
    bool canUnbind(int position, int length);
    void unbind(int position, int length);
    bool canEdit(int position);
    void edit(int position, QObject* currentApp = 0);
    bool canAdjust(int position);
    void adjust(int position, QObject* currentApp = 0);
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
    bool                       m_silentEditing;
    bool                       m_paletteEditing;
    QLivePaletteContainer*     m_paletteContainer;
    bool                       m_autoInserting;

    QDocumentCodeState*        m_state;
};

inline QQuickTextDocument *QDocumentCodeInterface::target(){
    return m_target;
}

inline lcv::QCodeCompletionModel *QDocumentCodeInterface::completionModel() const{
    return m_completionModel;
}

inline void QDocumentCodeInterface::enableSilentEditing(){
    m_silentEditing = true;
}

inline void QDocumentCodeInterface::disableSilentEditing(){
    m_silentEditing = false;
}

inline QDocumentCodeState *QDocumentCodeInterface::state(){
    return m_state;
}

}// namespace

#endif // QDOCUMENTCODEINTERFACE_H
