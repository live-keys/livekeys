
TEMPLATE = lib
TARGET   = qmljsparser
QT      += core qml quick xml
CONFIG  += qt c++11

linkLocalLibrary(../lcveditor, lcveditor)

DEFINES += Q_QMLJSPARSER_LIB

win32:CONFIG(debug, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../../application/debug)
else:win32:CONFIG(release, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../../application/release)
else:unix: TARGET = $$quote($$OUT_PWD/../../application/qmljsparser)

## Set the name of your library
#LIB_NAME = qmljsparser
#include($$getConfigFile(is_editor_lib.pri))
#include($$getConfigFile(use_lcveditor.pri))

#QT      += xml
#DEFINES += Q_QMLJSPARSER_LIB

#ENABLE_PLUGINTYPES = true

!isEmpty(ENABLE_PLUGINTYPES){
    message(Plugin Types Enabled: Live CV will use private libraries)
    QT += core-private qml-private quick-private
    DEFINES += Q_PLUGINTYPES_ENABLED
}

include($$PWD/src/qmljsparser.pri)
include($$PWD/3rdparty/3rdparty.pro)
include($$PWD/plugintypes/plugintypes.pri)
