TEMPLATE = lib
TARGET   = lcveditor
QT      += core qml quick
CONFIG  += qt

win32:{
    DESTDIR    = $$BUILD_PWD/lib
    DLLDESTDIR = $$DEPLOY_PWD
}else:DESTDIR = $$DEPLOY_PWD

DEFINES += Q_LCVEDITOR_LIB

include($$PWD/src/lcveditor.pri)
include($$PWD/include/lcveditorheaders.pri)
