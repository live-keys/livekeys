#ifndef QPROJECTIONSURFACE_H
#define QPROJECTIONSURFACE_H

#include <QObject>
#include "qvariant.h"
#include <QRectF>
#include "opencv2/imgproc.hpp"
#include <QJSValue>

class QMat;
class QProjectionMapper;

class QProjectionSurface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QMat*                input       READ input          WRITE   setInput        NOTIFY  inputChanged)
    Q_PROPERTY(QProjectionMapper*   mapper      READ mapper         CONSTANT)
    Q_PROPERTY(QJSValue             destination READ destination    WRITE   setDestination  NOTIFY destinationChanged)

public:
    explicit QProjectionSurface(QObject *parent = nullptr);
    ~QProjectionSurface();

    QMat *input() const;
    void setInput(QMat *input);

    QProjectionMapper *mapper() const;

    QJSValue destination() const;
    void setDestination(const QJSValue &destination);

signals:
    void inputChanged();
    void sourceChanged();
    void destinationChanged();
private:

    QJSValue m_destination;
    QMat* m_input;
};

#endif // QPROJECTIONSURFACE_H
