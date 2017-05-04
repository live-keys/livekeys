
PLUGIN_NAME = lcvimgproc
PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_IMGPROC/qml
include($$getConfigFile(is_plugin.pri))
include($$getConfigFile(use_plugin_core.pri))
include($$getConfigFile(use_plugin_live.pri))

#uri = plugins.lcvcore

DEFINES += Q_LCV

include($$PWD/src/lcvimgproc.pri)
include($$PWD/../../3rdparty/opencvconfig.pro)
deployOpenCV()

# Qml

OTHER_FILES = \
    $$PWD/qml/qmldir \
    $$PWD/qml/plugins.qmltypes
