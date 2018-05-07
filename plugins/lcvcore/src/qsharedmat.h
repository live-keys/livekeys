#ifndef QSHAREDMAT_H
#define QSHAREDMAT_H

#include <QObject>
#include "qmat.h"
#include "live/shareddata.h"

class Q_LCVCORE_EXPORT QSharedMat : public QMat, public lv::SharedData{

    Q_OBJECT

public:
    explicit QSharedMat(QObject *parent = 0);
    ~QSharedMat();
};

#endif // QSHAREDMAT_H
