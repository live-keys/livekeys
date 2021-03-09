#ifndef QFACELANDMARKS_H
#define QFACELANDMARKS_H

#include <QObject>
#include "opencv2/core.hpp"

class QFaceLandmarks : public QObject
{
    Q_OBJECT
public:
    explicit QFaceLandmarks(QObject *parent = nullptr);
public slots:

signals:

private:

};

#endif // QFACELANDMARKS_H
