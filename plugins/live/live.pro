
PLUGIN_NAME = live
PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_LIVE/qml
include($$getConfigFile(is_plugin.pri))

uri = plugins.live

DEFINES += Q_LIVE_LIB

include($$PWD/src/live.pri)
include($$PWD/include/liveheaders.pri)

OTHER_FILES *= \
    qml/*.qml \
    qmldir \
    projects.qmltypes

# Handling the palette

OTHER_FILES *= \
    palettes/palettedir \
    palettes/*.qml

PALETTES_DEPLOY_FROM         = $$shell_path($$PATH_SOURCE_PLUGINS_LIVE/palettes)
PLUGIN_DEPLOY_TO        = $$shell_path($$PATH_DEPLOY_PLUGINS/$$PLUGIN_NAME)

!exists($$PALETTES_DEPLOY_FROM){
    warning(Expected folder $$PALETTES_DEPLOY_FROM)
    copy.commands =
} else {
    copy.commands = $$QMAKE_COPY_DIR $$PALETTES_DEPLOY_FROM $$PLUGIN_DEPLOY_TO
}

palette_first.depends = $(palette_first) copy
export(palette_first.depends)
export(copy.commands)

QMAKE_EXTRA_TARGETS += palette_first copy
