TEMPLATE = lib
TARGET   = lcvlib
QT      += qml quick
CONFIG  += qt

OTHER_FILES = qmldir

DEFINES += Q_LCV
DEFINES += Q_LCV_LIB

CONFIG(debug, debug|release) {
    DLLDESTDIR += $$quote(debug/../../application/debug)
}
CONFIG(release, debug|release) {
    DLLDESTDIR += $$quote(release/../../application/release)
}

include($$PWD/src/lcvlib.pri)
include($$PWD/../../config.pro)
