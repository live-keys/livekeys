
PLUGIN_NAME = lcvlive
PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_LIVE/qml
include($$getConfigFile(is_plugin.pri))

uri = plugins.lcvlive

DEFINES += Q_LIVE_LIB

include($$PWD/src/live.pri)
include($$PWD/include/liveheaders.pri)

OTHER_FILES *= \
    qml/*.qml \
    qmldir \
    projects.qmltypes
