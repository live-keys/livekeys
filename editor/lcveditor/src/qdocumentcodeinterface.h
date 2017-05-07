#ifndef QDOCUMENTCODEINTERFACE_H
#define QDOCUMENTCODEINTERFACE_H

#include "qlcveditorglobal.h"
#include "qcodecompletionmodel.h"
#include "qabstractcodehandler.h"

#include <QObject>
#include <QTextCursor>

class QQuickTextDocument;

namespace lcv{

class Q_LCVEDITOR_EXPORT QDocumentCodeInterface : public QObject{

    Q_OBJECT
    Q_PROPERTY(QQuickTextDocument* target                 READ target          WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(lcv::QCodeCompletionModel* completionModel READ completionModel CONSTANT)

public:
    explicit QDocumentCodeInterface(QAbstractCodeHandler* handler, QObject* parent = 0);
    ~QDocumentCodeInterface();

    QQuickTextDocument *target();
    void setTarget(QQuickTextDocument *target);

    lcv::QCodeCompletionModel* completionModel() const;

    void enableSilentEditing();
    void disableSilentEditing();

    void rehighlightBlock(const QTextBlock& block);

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
    void edit(int position);

signals:
    void targetChanged();
    void cursorPositionRequest(int position);
    void contentsChangedManually();

private:
    QChar                      m_lastChar;
    QQuickTextDocument*        m_target;
    QTextDocument*             m_targetDoc;
    lcv::QCodeCompletionModel* m_completionModel;
    QAbstractCodeHandler*      m_codeHandler;
    QProjectDocument*          m_projectDocument;
    bool                       m_silentEditing;
    bool                       m_autoInserting;
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

}// namespace

#endif // QDOCUMENTCODEINTERFACE_H
