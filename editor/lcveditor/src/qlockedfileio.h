#ifndef QLOCKEDFILEIO_H
#define QLOCKEDFILEIO_H

#include "qlcveditorglobal.h"

#include <QSharedPointer>

namespace lcv{

class Q_LCVEDITOR_EXPORT QLockedFileIO{

    Q_DISABLE_COPY(QLockedFileIO)

public:
    typedef QSharedPointer<const QLockedFileIO> Ptr;
    typedef QSharedPointer<QLockedFileIO> MutablePtr;

public:
    ~QLockedFileIO();

    static MutablePtr createInstance();

    QString readFromFile(const QString& path);
    bool writeToFile(const QString& path, const QString& data);

private:
    QLockedFileIO();
};

}// namespace

#endif // QLOCKEDFILEIO_H
