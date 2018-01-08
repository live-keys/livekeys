PLUGIN_NAME = lcvphoto
PLUGIN_PATH = lcvphoto

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = lcvphoto

DEFINES += Q_LCV

# Dependencies

linkLocalPlugin(live, live)
linkLocalPlugin(lcvcore, lcvcore)

# Deploying qml is handled by the plugin.pri configuration

# Source

include($$PWD/src/lcvphoto.pri)
include($$PROJECT_ROOT/project/3rdparty/opencv.pri)

OTHER_FILES *= \
    qml/qmldir \
    qml/plugins.qmltypes

DISTFILES += \
    qml/HueSaturationLightnessSliders.qml
