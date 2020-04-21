#ifndef LVVIDEOSURFACE_H
#define LVVIDEOSURFACE_H

#include "qmatdisplay.h"

namespace lv{

class VideoSurface : public QMatDisplay{

    Q_OBJECT

public:
    explicit VideoSurface(QQuickItem *parent = nullptr);

    void updateSurface(qint64 cursorPosition, QMat* frame);

signals:

public slots:
};

}// namespace

#endif // LVVIDEOSURFACE_H
