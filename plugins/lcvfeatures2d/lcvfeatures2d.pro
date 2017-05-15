PLUGIN_NAME = lcvfeatures2d
PLUGIN_PATH = $$PWD

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getConfigFile(is_plugin.pri))

uri = plugins.lcvfeatures2d

DEFINES += Q_LCV
DEFINES += Q_LCVFEATURES2D_LIB

# Dependencies

linkLocalPlugin(live,    live)
linkLocalPlugin(lcvcore, lcvcore)

# Deploying qml is handled by the is_plugin configuration

# Source

include($$PWD/src/lcvfeatures2d.pri)
include($$PWD/include/lcvfeatures2dheaders.pri)
include($$PROJECT_ROOT/3rdparty/opencvconfig.pro)

deployOpenCV()

OTHER_FILES *= \
    qml/*.qml \
    qmldir \
    qml/plugins.qmltypes
