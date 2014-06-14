TEMPLATE = lib
TARGET   = lcvimgproc
QT      += qml quick
CONFIG  += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
uri = plugins.lcvcore

OTHER_FILES = qmldir

DEFINES += Q_LCV

REQUIRES_CV_IMGPROC = 1

include($$PWD/src/lcvimgproc.pri)
include($$PWD/../../config.pro)

INCLUDEPATH += $$PWD/../lcvlib/src
DEPENDPATH  += $$PWD/../lcvlib/src

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lcvlib/release/ -llcvlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lcvlib/debug/ -llcvlib
else:unix: LIBS += -L$$OUT_PWD/../lcvlib/ -llcvlib

CONFIG(debug, debug|release){
	DESTDIR = $$quote(debug/../../application/debug/plugins/lcvimgproc)
}
CONFIG(release, debug|release){
	DESTDIR = $$quote(release/../../application/release/plugins/lcvimgproc)
}

!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$DESTDIR/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"
    QMAKE_EXTRA_TARGETS += copy_qmldir
    PRE_TARGETDEPS += $$copy_qmldir.target
}

qmldir.files = qmldir
unix {
    installPath = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    qmldir.path = $$installPath
    target.path = $$installPath
    INSTALLS += target qmldir
}
