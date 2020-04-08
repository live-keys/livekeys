#include "videosurface.h"

#include <QQmlEngine>

namespace lv{

VideoSurface::VideoSurface(QQuickItem *parent)
    : QMatDisplay(parent)
{
}

void VideoSurface::updateSurface(qint64 /*cursorPosition*/, QMat *frame){
    QQmlEngine::setObjectOwnership(frame, QQmlEngine::CppOwnership);
    setOutput(frame);
    update();
}

}// namespace
