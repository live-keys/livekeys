TEMPLATE = lib
TARGET   = qmljsparser
QT      += core qml quick xml core-private qml-private quick-private
CONFIG  += qt c++11

DEFINES += Q_QMLJSPARSER_LIB

# Destination

win32:CONFIG(debug, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../application/debug)
else:win32:CONFIG(release, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../application/release)
else:unix: TARGET = $$quote($$OUT_PWD/../application/qmljsparser)

include($$PWD/src/qmljsparser.pri)
include($$PWD/3rdparty/3rdparty.pro)
include($$PWD/plugintypes/plugintypes.pri)
