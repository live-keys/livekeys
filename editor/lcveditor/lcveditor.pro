TEMPLATE = lib
TARGET   = lcveditor
QT      += core qml quick
CONFIG  += qt

win32:CONFIG(debug, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../../application/debug)
else:win32:CONFIG(release, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../../application/release)
else:unix: TARGET = $$quote($$OUT_PWD/../../application/lcveditor)

DEFINES += Q_LCVEDITOR_LIB

include($$PWD/src/lcveditor.pri)
