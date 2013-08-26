#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "QMat.hpp"
#include "QMatSource.hpp"
#include "QChannelSelect.hpp"
#include "QCursorShape.hpp"
#include "QCanny.hpp"

#include <QDebug>

int main(int argc, char *argv[]){

    QGuiApplication app(argc, argv);

    qmlRegisterType<QCursorShape>  ("Cv", 1, 0, "CursorArea");
    qmlRegisterType<QMat>          ("Cv", 1, 0, "Mat");
    qmlRegisterType<QMatSource>    ("Cv", 1, 0, "MatSource");
    qmlRegisterType<QChannelSelect>("Cv", 1, 0, "ChannelSelect");
    qmlRegisterType<QCanny>        ("Cv", 1, 0, "Canny");

    QtQuick2ApplicationViewer viewer;
    viewer.setMinimumSize(QSize(800, 600));
    viewer.setMainQmlFile(QStringLiteral("qml/LiveCV/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
