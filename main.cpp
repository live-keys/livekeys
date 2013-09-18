#include <QtGui/QGuiApplication>
#include <QQmlContext>
#include "qtquick2applicationviewer.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "QMat.hpp"
#include "QMatSource.hpp"
#include "QMatEmpty.hpp"
#include "QMatRoi.hpp"
#include "QMatRead.hpp"
#include "QMat2DArray.hpp"
#include "QCamCapture.hpp"
#include "QVideoCapture.hpp"
#include "QChannelSelect.hpp"
#include "QCursorShape.hpp"
#include "QCanny.hpp"
#include "QCodeDocument.hpp"
#include "QCornerEigenValsAndVecs.hpp"
#include "QThreshold.hpp"
#include "QAlphaMerge.hpp"
#include "QSobel.hpp"

#include <QDebug>

int main(int argc, char *argv[]){

    QGuiApplication app(argc, argv);

    qmlRegisterType<QCursorShape>  ("Cv", 1, 0, "CursorArea");
    qmlRegisterType<QMat>          ("Cv", 1, 0, "Mat");
    qmlRegisterType<QMatSource>    ("Cv", 1, 0, "MatSource");
    qmlRegisterType<QMatEmpty>     ("Cv", 1, 0, "MatEmpty");
    qmlRegisterType<QMatRoi>       ("Cv", 1, 0, "MatRoi");
    qmlRegisterType<QMatRead>      ("Cv", 1, 0, "MatRead");
    qmlRegisterType<QMat2DArray>   ("Cv", 1, 0, "Mat2DArray");
    qmlRegisterType<QCodeDocument> ("Cv", 1, 0, "Document");
    qmlRegisterType<QChannelSelect>("Cv", 1, 0, "ChannelSelect");
    qmlRegisterType<QCanny>        ("Cv", 1, 0, "Canny");
    qmlRegisterType<QCamCapture>   ("Cv", 1, 0, "CamCapture");
    qmlRegisterType<QVideoCapture> ("Cv", 1, 0, "VideoCapture");
    qmlRegisterType<QThreshold>    ("Cv", 1, 0, "Threshold");
    qmlRegisterType<QAlphaMerge>   ("Cv", 1, 0, "AlphaMerge");
    qmlRegisterType<QSobel>        ("Cv", 1, 0, "Sobel");

    qmlRegisterType<QCornerEigenValsAndVecs>("Cv", 1, 0, "CornerEigenValsAndVecs");

    QCodeDocument document;
    QtQuick2ApplicationViewer viewer;
    viewer.setMinimumSize(QSize(800, 600));
    viewer.rootContext()->setContextProperty("codeDocument", &document);
    viewer.setMainQmlFile(QStringLiteral("qml/LiveCV/main.qml"));
    document.setEngine(viewer.engine());
    viewer.showExpanded();

    return app.exec();
}
