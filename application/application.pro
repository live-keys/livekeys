TEMPLATE = app
TARGET   = livecv
QT      += qml quick

linkLocalLibrary(../editor/lcveditor, lcveditor)
linkLocalLibrary(../editor/qmljsparser, qmljsparser)
linkLocalLibrary(../plugins/live, live)

#DESTDIR = $$PATH_DEPLOY_APPLICATION

# Load library paths
# ------------------

unix{
    QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\''
}

##TODO: Revert
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

# Application
# -----------

include($$PWD/src/base/base.pri)
include($$PWD/src/main/main.pri)

RC_ICONS = $$PWD/icons/livecv.ico

RESOURCES += $$PWD/application.qrc

OTHER_FILES += \
    $$PWD/qml/*.qml


