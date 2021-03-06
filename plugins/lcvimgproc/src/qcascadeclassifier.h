#ifndef QCASCADECLASSIFIER_H
#define QCASCADECLASSIFIER_H

#include <QObject>
#include "opencv2/objdetect.hpp"
#include "qmat.h"
#include <QVariantList>

class QCascadeClassifier: public QObject
{
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_OBJECT
public:
    QCascadeClassifier();

    void setFile(QString& fileString);
    QString file();
public slots:
    QVariantList detectFaces(QMat* input, double scaleFactor = 1.1, int minNeighbors = 3, int flags = 0);
signals:
    void fileChanged();
private:
    cv::CascadeClassifier* m_cc;
    QString m_file;
};

inline QString QCascadeClassifier::file()
{
    return m_file;
}

#endif // QCASCADECLASSIFIER_H
