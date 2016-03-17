TEMPLATE = lib
TARGET   = lcvcore
QT      += qml quick
CONFIG  += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
uri = plugins.lcvcore

OTHER_FILES = qmldir

DEFINES += Q_LCV

include($$PWD/src/lcvcore.pri)
include($$PWD/../../3rdparty/opencvconfig.pro)
loadOpenCV(core highgui, deploy)

INCLUDEPATH += $$PWD/../../lib/include
DEPENDPATH  += $$PWD/../../lib/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/release/ -llcvlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/debug/ -llcvlib
else:unix: LIBS += -L$$OUT_PWD/../../application/ -llcvlib

# Destination

win32:CONFIG(debug, debug|release): DESTDIR = $$quote($$OUT_PWD/../../application/debug/plugins/lcvcore)
else:win32:CONFIG(release, debug|release): DESTDIR = $$quote($$OUT_PWD/../../application/release/plugins/lcvcore)
else:unix: DESTDIR = $$quote($$OUT_PWD/../../application/plugins/lcvcore)


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
