PLUGIN_NAME = lcvvideo
PLUGIN_PATH = $$PWD

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getConfigFile(is_plugin.pri))

uri = plugins.lcvvideo

DEFINES += Q_LCV
DEFINES += Q_LCVVIDEO_LIB

# Dependencies

linkLocalPlugin(live, live)
linkLocalPlugin(lcvcore, lcvcore)

# Deploying qml is handled by the is_plugin configuration

# Source

include($$PWD/src/lcvvideo.pri)
include($$PWD/include/lcvvideoheaders.pri)
include($$PROJECT_ROOT/3rdparty/opencvconfig.pro)

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes
