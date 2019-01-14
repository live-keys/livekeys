PLUGIN_NAME = lcvvideo
PLUGIN_PATH = lcvvideo

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = lcvvideo

DEFINES += Q_LCV
DEFINES += Q_LCVVIDEO_LIB

# Dependencies

linkLocalPlugin(live, live)
linkLocalPlugin(lcvcore, lcvcore)

# Deploying qml is handled by the plugin.pri configuration

# Source

include($$PWD/src/lcvvideo.pri)
include($$PWD/include/lcvvideoheaders.pri)
include($$PROJECT_ROOT/project/3rdparty/opencv.pri)

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes \
    doc/*.md

DISTFILES += \
    qml/live.package.json \
    qml/live.plugin.json
