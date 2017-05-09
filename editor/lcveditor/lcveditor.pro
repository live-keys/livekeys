TEMPLATE = lib
TARGET   = lcveditor
QT      += core qml quick
CONFIG  += qt

win32:DLLDESTDIR = $$buildModePath($$DEPLOY_PWD)
else:DESTDIR = $$buildModePath($$DEPLOY_PWD)

DEFINES += Q_LCVEDITOR_LIB

include($$PWD/src/lcveditor.pri)
