
# Set the name of your library
LIB_NAME = qmljsparser
include($$getConfigFile(is_editor_lib.pri))
include($$getConfigFile(use_lcveditor.pri))

QT      += xml
DEFINES += Q_QMLJSPARSER_LIB

#ENABLE_PLUGINTYPES = true

!isEmpty(ENABLE_PLUGINTYPES){
    message(Plugin Types Enabled: Live CV will use private libraries)
    QT += core-private qml-private quick-private
    DEFINES += Q_PLUGINTYPES_ENABLED
}

include($$PWD/src/qmljsparser.pri)
include($$PWD/3rdparty/3rdparty.pro)
include($$PWD/plugintypes/plugintypes.pri)
