TEMPLATE = lib
TARGET   = lcvlib
QT      += qml quick
CONFIG  += qt

OTHER_FILES = qmldir

DEFINES += Q_LCV
DEFINES += Q_LCV_LIB

# Destination

win32:CONFIG(debug, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../application/debug)
else:win32:CONFIG(release, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../application/release)
else:unix: TARGET = $$quote($$OUT_PWD/../application/lcvlib)

include($$PWD/src/lcvlibsrc.pri)
include($$PWD/include/lcvlibinclude.pri)
include($$PWD/../3rdparty/opencvconfig.pro)
loadOpenCV(core)

