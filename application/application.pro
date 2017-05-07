TEMPLATE = app
TARGET   = livecv
CONFIG  += c++11
QT      += qml quick

# Editor library
# --------------

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../editor/lcveditor/release/ -llcveditor
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../editor/lcveditor/debug/ -llcveditor
else:unix: LIBS += -L$$OUT_PWD/../application -llcveditor

INCLUDEPATH += $$PWD/../editor/lcveditor/src
DEPENDPATH += $$PWD/../editor/lcveditor/src

# QmlJS library
# -------------

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../editor/qmljsparser/release/ -lqmljsparser
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../editor/qmljsparser/debug/ -lqmljsparser
else:unix: LIBS += -L$$OUT_PWD/../application -lqmljsparser

INCLUDEPATH += $$PWD/../editor/qmljsparser/src
DEPENDPATH += $$PWD/../editor/qmljsparser/src

# Live library
# ------------

INCLUDEPATH += $$PWD/../lib/include
DEPENDPATH  += $$PWD/../lib/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -llive
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -llive
else:unix: LIBS += -L$$OUT_PWD/../application/ -llive

# Load library paths
# ------------------

unix{
    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
}

# Application
# -----------

include($$PWD/src/base/base.pri)
include($$PWD/src/main/main.pri)

RC_ICONS = $$PWD/icons/livecv.ico

win32{
    Release:DESTDIR = release/../release #fix for app current path
    Release:OBJECTS_DIR = release/.obj
    Release:MOC_DIR = release/.moc
    Release:RCC_DIR = release/.rcc

    Debug:DESTDIR = debug/../debug #fix for app current path
    Debug:OBJECTS_DIR = debug/.obj
    Debug:MOC_DIR = debug/.moc
    Debug:RCC_DIR = debug/.rcc
}

# Qml
# ---

RESOURCES += $$PWD/application.qrc

include(deployment.pri)

