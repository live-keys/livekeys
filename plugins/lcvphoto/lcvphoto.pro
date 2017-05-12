PLUGIN_NAME = lcvphoto
PLUGIN_PATH = $$PWD

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getConfigFile(is_plugin.pri))

uri = plugins.lcvphoto

DEFINES += Q_LCV

# Destination

win32:DLLDESTDIR = $$buildModePath($$DEPLOY_PWD)/plugins/$$PLUGIN_PATH
else:DESTDIR = $$buildModePath($$DEPLOY_PWD)/plugins/$$PLUGIN_PATH

# Dependencies

linkLocalPlugin(live, live)
linkLocalPlugin(lcvcore, lcvcore)

# Deploying qml is handled by the is_plugin configuration

# Source

include($$PWD/src/lcvphoto.pri)
include($$PROJECT_ROOT/3rdparty/opencvconfig.pro)
deployOpenCV()

OTHER_FILES *= \
    qml/qmldir \
    qml/plugins.qmltypes
