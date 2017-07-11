PLUGIN_NAME = lcvimgproc
PLUGIN_PATH = lcvimgproc

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = lcvimgproc

DEFINES += Q_LCV

# Dependencies

linkLocalPlugin(live, live)
linkLocalPlugin(lcvcore, lcvcore)

# Deploying qml is handled by the plugin.pri configuration

# Source

include($$PWD/src/lcvimgproc.pri)
include($$PROJECT_ROOT/project/3rdparty/opencv.pri)

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes
