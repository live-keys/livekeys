TEMPLATE = lib
TARGET   = lcveditor
QT      += core qml quick
CONFIG  += qt c++11

DEFINES += Q_LCVEDITOR_LIB

# Destination

win32:CONFIG(debug, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../../application/debug)
else:win32:CONFIG(release, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../../application/release)
else:unix: TARGET = $$quote($$OUT_PWD/../../application/lcveditor)

include($$PWD/src/lcveditor.pri)
