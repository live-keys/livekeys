TEMPLATE = lib
TARGET   = lveditor
QT      += core qml quick
CONFIG  += qt

linkLocalLibrary(lvbase, lvbase)

win32:{
    DESTDIR    = $$DEPLOY_PATH/dev/lib
    DLLDESTDIR = $$DEPLOY_PATH
}else:DESTDIR  = $$DEPLOY_PATH

DEFINES += LV_EDITOR_LIB

include($$PWD/src/lveditor.pri)
include($$PWD/include/lveditorheaders.pri)
