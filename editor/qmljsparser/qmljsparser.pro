
## Set the name of your library
#LIB_NAME = qmljsparser
#include($$getConfigFile(is_editor_lib.pri))
#include($$getConfigFile(use_lcveditor.pri))
#QT      += xml

TEMPLATE = lib
TARGET   = qmljsparser
QT      += core qml quick xml
CONFIG  += qt c++11

linkLocalLibrary(editor/lcveditor, lcveditor)

DEFINES += Q_QMLJSPARSER_LIB

win32:DLLDESTDIR = $$buildModePath($$DEPLOY_PWD)
else:DESTDIR = $$buildModePath($$DEPLOY_PWD)


#ENABLE_PLUGINTYPES = true

!isEmpty(ENABLE_PLUGINTYPES){
    message(Plugin Types Enabled: Live CV will use private libraries)
    QT += core-private qml-private quick-private
    DEFINES += Q_PLUGINTYPES_ENABLED
}

include($$PWD/src/qmljsparser.pri)
include($$PWD/3rdparty/3rdparty.pro)
include($$PWD/plugintypes/plugintypes.pri)
