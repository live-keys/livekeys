TEMPLATE = app
TARGET   = livecv
CONFIG  += c++11
QT      += qml quick

include($$getConfigFile(use_lcveditor.pri))
include($$getConfigFile(use_qmljsparser.pri))

# Live library
# ------------

INCLUDEPATH += $$PWD/../lib/live/include
DEPENDPATH  += $$PWD/../lib/live/include

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

