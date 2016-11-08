#ifndef QQMLCOMPLETIONCONTEXTFINDER_H
#define QQMLCOMPLETIONCONTEXTFINDER_H

#include "qqmlcompletioncontext.h"

class QTextCursor;
class QQmlCompletionContextFinder{

public:
    QQmlCompletionContextFinder();
    ~QQmlCompletionContextFinder();

    QQmlCompletionContext getContext(const QTextCursor& cursor);
};

#endif // QQMLCOMPLETIONCONTEXTFINDER_H
