PLUGIN_NAME = lcvphoto
PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_PHOTO/qml
include($$getConfigFile(is_plugin.pri))
include($$getConfigFile(use_plugin_core.pri))
include($$getConfigFile(use_plugin_live.pri))

uri = plugins.lcvcore

DEFINES += Q_LCV

include($$PWD/src/lcvphoto.pri)
include($$PWD/../../3rdparty/opencvconfig.pro)
deployOpenCV()

OTHER_FILES *= \
    qml/*.qml \
    qmldir \
    projects.qmltypes
