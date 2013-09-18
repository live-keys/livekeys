include(Config.pro)

# Add more folders to ship with the application, here
folder_01.source = qml/LiveCV
folder_01.target = qml
folder_02.source = qml/LiveCVImages
folder_02.target = qml
DEPLOYMENTFOLDERS += folder_01 folder_02

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquickviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

QT += widgets

INCLUDEPATH += src/core
INCLUDEPATH += src/transform
INCLUDEPATH += src/view

SOURCES += main.cpp \
	src/core/QMatState.cpp \
	src/core/QMat.cpp \
	src/core/QMatSource.cpp \
	src/core/QMatNode.cpp \
	src/core/QMatShader.cpp \
	src/core/QMatDisplay.cpp \
	src/view/QCursorShape.cpp \
	src/transform/QChannelSelect.cpp \
	src/transform/QCanny.cpp \
        src/view/QCodeDocument.cpp \
    src/transform/QCornerEigenValsAndVecs.cpp \
    src/core/QCamCapture.cpp \
    src/core/QCaptureContainer.cpp \
    src/core/QCamCaptureThread.cpp \
    src/core/QVideoCapture.cpp \
    src/core/QVideoCaptureThread.cpp \
    src/core/QMatEmpty.cpp \
    src/transform/QThreshold.cpp \
    src/transform/QAlphaMerge.cpp \
    src/core/QMatRoi.cpp \
    src/core/QMatRead.cpp \
    src/core/QMat2DArray.cpp \
    src/transform/QGaussianBlur.cpp \
    src/core/QMatFilter.cpp \
    src/transform/QSobel.cpp

HEADERS += \
	src/core/QMatState.hpp \
	src/core/QMat.hpp \
	src/core/QMatSource.hpp \
	src/core/QMatNode.hpp \
	src/core/QMatShader.hpp \
	src/core/QMatDisplay.hpp \
	src/view/QCursorShape.hpp \
	src/transform/QChannelSelect.hpp \
	src/transform/QCanny.hpp \
        src/view/QCodeDocument.hpp \
    src/transform/QCornerEigenValsAndVecs.hpp \
    src/core/QCamCapture.hpp \
    src/core/QCaptureContainer.hpp \
    src/core/QCamCaptureThread.hpp \
    src/core/QVideoCapture.hpp \
    src/core/QVideoCaptureThread.hpp \
    src/core/QMatEmpty.hpp \
    src/transform/QThreshold.hpp \
    src/transform/QAlphaMerge.hpp \
    src/core/QMatRoi.hpp \
    src/core/QMatRead.hpp \
    src/core/QMat2DArray.hpp \
    src/transform/QGaussianBlur.hpp \
    src/core/QMatFilter.hpp \
    src/transform/QSobel.hpp
