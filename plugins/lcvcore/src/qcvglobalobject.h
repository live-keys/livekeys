#ifndef QCVGLOBALOBJECT_H
#define QCVGLOBALOBJECT_H

#include <QObject>
#include "qmat.h"

class QCvGlobalObject : public QObject{

    Q_OBJECT
    Q_PROPERTY(QMat* nullMat READ nullMat CONSTANT)

public:
    explicit QCvGlobalObject(QObject *parent = nullptr);

    QMat* nullMat() const;

public slots:
    QVariantList matToArray(QMat* m);
    void assignArrayToMat(const QVariantList &a, QMat *m);
};

inline QMat *QCvGlobalObject::nullMat() const{
    return QMat::nullMat();
}

#endif // QCVGLOBALOBJECT_H
