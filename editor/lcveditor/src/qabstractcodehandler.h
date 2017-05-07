#ifndef QABSTRACTCODEHANDLER_H
#define QABSTRACTCODEHANDLER_H

#include <QObject>
#include "qlcveditorglobal.h"
#include "qcodecompletionmodel.h"

class QTextDocument;
class QTextCursor;
class QTextBlock;

namespace lcv{

class QProjectDocument;
class QProjectDocumentBinding;

class Q_LCVEDITOR_EXPORT QAbstractCodeHandler : public QObject{

    Q_OBJECT

public:
    class CodeProperty{
    public:
        CodeProperty(int pPosition, int pLength, const QStringList& pName, const QString& pType)
            : position(pPosition)
            , length(pLength)
            , name(pName)
            , type(pType)
        {}

        int position;
        int length;
        QStringList name;
        QString type;
    };

public:
    explicit QAbstractCodeHandler(QObject* parent = 0);
    virtual ~QAbstractCodeHandler();

    virtual void setTarget(QTextDocument* target) = 0;
    virtual void assistCompletion(
        const QTextCursor& cursor,
        const QChar& insertion,
        bool manuallyTriggered,
        QCodeCompletionModel* model,
        QTextCursor& cursorChange
    ) = 0;
    virtual void setDocument(QProjectDocument* document) = 0;
    virtual void updateScope(const QString& data) = 0;
    virtual void rehighlightBlock(const QTextBlock &block) = 0;
    virtual QList<CodeProperty> getProperties(const QTextCursor& cursor) = 0;
    virtual void connectBindings(QList<QProjectDocumentBinding*> bindings, QObject* root) = 0;
};

}// namespace

#endif // QABSTRACTCODEHANDLER_H
