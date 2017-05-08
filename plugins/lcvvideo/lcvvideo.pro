PLUGIN_NAME = lcvvideo
PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_VIDEO/qml
include($$getConfigFile(is_plugin.pri))
include($$getConfigFile(use_plugin_core.pri))
include($$getConfigFile(use_plugin_live.pri))

# uri = plugins.lcvvideo

DEFINES += Q_LCV
DEFINES += Q_LCVVIDEO_LIB

include($$PWD/src/lcvvideo.pri)
include($$PWD/include/lcvvideoheaders.pri)
include($$PWD/../../3rdparty/opencvconfig.pro)
deployOpenCV()

OTHER_FILES *= \
    qml/*.qml \
    qmldir \
    projects.qmltypes
