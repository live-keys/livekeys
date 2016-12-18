#ifndef QABSTRACTCODEHANDLER_H
#define QABSTRACTCODEHANDLER_H

#include <QObject>
#include "qlcveditorglobal.h"
#include "qcodecompletionmodel.h"

class QTextDocument;
class QTextCursor;

namespace lcv{

class QProjectDocument;

class Q_LCVEDITOR_EXPORT QAbstractCodeHandler : public QObject{

    Q_OBJECT

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
};

}// namespace

#endif // QABSTRACTCODEHANDLER_H
