TEMPLATE = lib
TARGET   = lcvphoto
QT      += qml quick
CONFIG  += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
uri = plugins.lcvcore

OTHER_FILES = qmldir

DEFINES += Q_LCV

include($$PWD/src/lcvphoto.pri)
include($$PWD/../../3rdparty/opencvconfig.pro)
loadOpenCV(core highgui photo, deploy)

INCLUDEPATH += $$PWD/../../lib/include
DEPENDPATH  += $$PWD/../../lib/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../lib/release/ -llcvlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../lib/debug/ -llcvlib
else:unix: LIBS += -L$$OUT_PWD/../../application/ -llcvlib

# Destination

win32:CONFIG(debug, debug|release): DESTDIR = $$quote($$OUT_PWD/../../application/debug/plugins/lcvphoto)
else:win32:CONFIG(release, debug|release): DESTDIR = $$quote($$OUT_PWD/../../application/release/plugins/lcvphoto)
else:unix: DESTDIR = $$quote($$OUT_PWD/../../application/plugins/lcvphoto)


!equals(_PRO_FILE_PWD_, $$OUT_PWD) {
    copy_qmldir.target = $$DESTDIR/qmldir
    copy_qmldir.depends = $$_PRO_FILE_PWD_/qmldir
    copy_qmldir.commands = $(COPY_FILE) \"$$replace(copy_qmldir.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmldir.target, /, $$QMAKE_DIR_SEP)\"

    copy_qmltypes.target   = $$DESTDIR/plugins.qmltypes
    copy_qmltypes.depends  = $$_PRO_FILE_PWD_/plugins.qmltypes
    copy_qmltypes.commands = $(COPY_FILE) \"$$replace(copy_qmltypes.depends, /, $$QMAKE_DIR_SEP)\" \"$$replace(copy_qmltypes.target, /, $$QMAKE_DIR_SEP)\"

    QMAKE_EXTRA_TARGETS += copy_qmldir copy_qmltypes
    PRE_TARGETDEPS += $$copy_qmldir.target $$copy_qmltypes.target
}

lcvphoto.files = qmldir plugins.qmltypes
unix {
    installPath   = $$[QT_INSTALL_QML]/$$replace(uri, \\., /)
    lcvphoto.path = $$installPath
    target.path   = $$installPath
    INSTALLS      += target lcvphoto
}
