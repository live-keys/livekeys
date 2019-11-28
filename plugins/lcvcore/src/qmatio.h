#ifndef QMATIO_H
#define QMATIO_H

#include <QObject>
#include "qmat.h"

/// \private
class QMatIO : public QObject{

    Q_OBJECT

    Q_ENUMS(Load)

public:
    enum Load{
        CV_LOAD_IMAGE_UNCHANGED  = -1,
        CV_LOAD_IMAGE_GRAYSCALE  =  0,
        CV_LOAD_IMAGE_COLOR      =  1,
        CV_LOAD_IMAGE_ANYDEPTH   =  2,
        CV_LOAD_IMAGE_ANYCOLOR   =  4
    };

public:
    explicit QMatIO(QObject *parent = nullptr);

public slots:
    QMat* read(const QString& path, int isColor = CV_LOAD_IMAGE_COLOR);
    QMat* decode(const QByteArray& bytes, int isColor = CV_LOAD_IMAGE_COLOR);

};

#endif // QMATIO_H
