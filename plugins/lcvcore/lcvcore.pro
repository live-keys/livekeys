PLUGIN_NAME = lcvcore
PLUGIN_PATH = $$PWD

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getConfigFile(is_plugin.pri))

uri = plugins.lcvcore

DEFINES += Q_LCV
DEFINES += Q_LCVCORE_LIB

# Destination

win32:DLLDESTDIR = $$buildModePath($$DEPLOY_PWD)/plugins/$$PLUGIN_PATH
else:DESTDIR = $$buildModePath($$DEPLOY_PWD)/plugins/$$PLUGIN_PATH

# Dependencies

linkLocalPlugin(plugins/live, live)

# Deploying qml is handled by the is_plugin configuration

# Source

include($$PWD/src/lcvcore.pri)
include($$PWD/include/lcvcoreheaders.pri)
include($$PROJECT_ROOT/3rdparty/opencvconfig.pro)
deployOpenCV()

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes


