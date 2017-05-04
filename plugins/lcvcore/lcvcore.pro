
PLUGIN_NAME = lcvcore
PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_CORE/qml

include($$getConfigFile(is_plugin.pri))
include($$getConfigFile(use_plugin_live.pri))

# TODO find out if this has any effect at all
#uri = plugins.lcvcore

DEFINES += Q_LCV
DEFINES += Q_LCVCORE_LIB

include($$PWD/src/lcvcore.pri)
include($$PWD/include/lcvcoreheaders.pri)
include($$PWD/../../3rdparty/opencvconfig.pro)
deployOpenCV()

# Qml

OTHER_FILES += \
    $$PWD/qml/qmldir \
    $$PWD/qml/plugins.qmltypes \
    $$PWD/qml/RegionSelection.qml \
    $$PWD/qml/VideoControls.qml


