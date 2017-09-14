TEMPLATE = lib
TARGET   = lcveditor
QT      += core qml quick
CONFIG  += qt

linkLocalLibrary(lib/lvbase, lvbase)

win32:{
    DESTDIR    = $$DEPLOY_PATH/dev/lib
    DLLDESTDIR = $$DEPLOY_PATH
}else:DESTDIR = $$DEPLOY_PATH

DEFINES += Q_LCVEDITOR_LIB

include($$PWD/src/lcveditor.pri)
include($$PWD/include/lcveditorheaders.pri)
