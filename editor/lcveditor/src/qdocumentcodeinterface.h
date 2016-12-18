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

public slots:
    void insertCompletion(int from, int to, const QString& completion);
    void documentContentsChanged(int position, int charsRemoved, int charsAdded);
    void cursorWritePositionChanged(QTextCursor cursor);
    void setDocument(QProjectDocument* document);
    void generateCompletion(int cursorPosition);
    void updateScope(const QString& data);

signals:
    void targetChanged();
    void cursorPositionRequest(int position);

private:
    QChar                      m_lastChar;
    QQuickTextDocument*        m_target;
    QTextDocument*             m_targetDoc;
    lcv::QCodeCompletionModel* m_completionModel;
    QAbstractCodeHandler*      m_codeHandler;
    bool                       m_autoInserting;
};

inline QQuickTextDocument *QDocumentCodeInterface::target(){
    return m_target;
}

inline lcv::QCodeCompletionModel *QDocumentCodeInterface::completionModel() const{
    return m_completionModel;
}

}// namespace

#endif // QDOCUMENTCODEINTERFACE_H
