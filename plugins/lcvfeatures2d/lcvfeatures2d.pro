PLUGIN_NAME = lcvfeatures2d
PLUGIN_PATH = lcvfeatures2d

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = lcvfeatures2d

DEFINES += Q_LCV
DEFINES += Q_LCVFEATURES2D_LIB

# Dependencies

linkLocalLibrary(lvbase, lvbase)
linkLocalLibrary(lvview, lvview)

linkLocalPlugin(live,    live)
linkLocalPlugin(lcvcore, lcvcore)

# Deploying qml is handled by the plugin.pri configuration

# Source

include($$PWD/src/lcvfeatures2d.pri)
include($$PWD/include/lcvfeatures2dheaders.pri)
include($$PROJECT_ROOT/project/3rdparty/opencv.pri)

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes \
    doc/*.md

DISTFILES += \
    qml/live.plugin.json \
    qml/live.package.json
