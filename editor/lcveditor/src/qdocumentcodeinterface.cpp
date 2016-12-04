#include "qdocumentcodeinterface.h"
#include <QQuickTextDocument>
#include <QTextDocument>
#include <QTextCursor>

namespace lcv{

QDocumentCodeInterface::QDocumentCodeInterface(QAbstractCodeHandler *handler, QObject *parent)
    : QObject(parent)
    , m_target(0)
    , m_targetDoc(0)
    , m_completionModel(new QCodeCompletionModel)
    , m_codeHandler(handler)
{
}

QDocumentCodeInterface::~QDocumentCodeInterface(){

}

void QDocumentCodeInterface::setTarget(QQuickTextDocument *target){
    if ( m_target != target ){
        m_target = target;
        if ( m_codeHandler ){
            m_targetDoc = 0;
            if ( target ){
                m_targetDoc = target->textDocument();
                if ( m_targetDoc ){
                    connect(
                        m_targetDoc, SIGNAL(cursorPositionChanged(QTextCursor)),
                        this, SLOT(cursorWritePositionChanged(QTextCursor))
                    );
                    connect(
                        m_targetDoc, SIGNAL(contentsChange(int,int,int)),
                        this, SLOT(documentContentsChanged(int,int,int))
                    );
                }
            }
            m_codeHandler->setTarget(m_targetDoc);
        }
        emit targetChanged();
    }
}

void QDocumentCodeInterface::insertCompletion(int from, int to, const QString &completion){
    if ( m_targetDoc ){
        QTextCursor cursor(m_targetDoc);
        cursor.beginEditBlock();
        cursor.setPosition(from);
        cursor.setPosition(to, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
        cursor.insertText(completion);
        cursor.endEditBlock();
    }
}

void QDocumentCodeInterface::documentContentsChanged(int position, int, int charsAdded){
    m_lastChar = QChar();
    if ( charsAdded == 1 && m_targetDoc ){
        m_lastChar = m_targetDoc->characterAt(position);
    }
}

void QDocumentCodeInterface::cursorWritePositionChanged(QTextCursor cursor){
    if ( m_codeHandler ){
        QTextCursor newCursor;
        m_codeHandler->assistCompletion(
            cursor,
            m_lastChar,
            false,
            m_completionModel,
            newCursor
        );
        if ( !newCursor.isNull() ){
            emit cursorPositionRequest(newCursor.position());
        }
    }
}

void QDocumentCodeInterface::setDocument(QProjectDocument *document){
    m_codeHandler->setDocument(document);
}

void QDocumentCodeInterface::generateCompletion(int cursorPosition){
    if ( m_target && m_codeHandler ){
        QTextCursor cursor(m_target->textDocument());
        cursor.setPosition(cursorPosition);
        m_codeHandler->assistCompletion(
            cursor,
            m_lastChar,
            true,
            m_completionModel,
            QTextCursor()
        );
    }
}

void QDocumentCodeInterface::updateScope(const QString &data){
    m_codeHandler->updateScope(data);
}

}// namespace
