include($$configFile(config_functions.pri))
include($$configFile(config_paths.pri))

TEMPLATE = lib
TARGET   = lcveditor
QT      += core qml quick
CONFIG  += qt c++11

DEFINES += Q_LCVEDITOR_LIB

# Destination

win32:DLLDESTDIR = $$PATH_BUILD_APPLICATION

else:unix: TARGET = $$quote($$OUT_PWD/../../application/lcveditor)

include($$PWD/src/lcveditor.pri)
