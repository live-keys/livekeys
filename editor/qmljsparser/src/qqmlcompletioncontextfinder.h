#ifndef QQMLCOMPLETIONCONTEXTFINDER_H
#define QQMLCOMPLETIONCONTEXTFINDER_H

#include "qqmlcompletioncontext.h"

class QTextCursor;

namespace lcv{

class Q_QMLJSPARSER_EXPORT QQmlCompletionContextFinder{

public:
    QQmlCompletionContextFinder();
    ~QQmlCompletionContextFinder();

    QQmlCompletionContext* getContext(const QTextCursor& cursor);
};

}// namespace

#endif // QQMLCOMPLETIONCONTEXTFINDER_H
