PLUGIN_NAME = live
PLUGIN_PATH = $$PWD

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getConfigFile(is_plugin.pri))

DEFINES += Q_LIVE_LIB

uri = plugins.live

linkLocalLibrary(editor/lcveditor, lcveditor)
# Source

include($$PWD/src/live.pri)
include($$PWD/include/liveheaders.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes

# Handling the palette

OTHER_FILES *= \
    palettes/palettedir \
    palettes/*.qml

# Deploy The palette

palettecopy.commands = $$deployDirCommand($$PWD/palettes, $$PATH_DEPLOY_PLUGINS/$$PLUGIN_NAME)
first.depends = $(first) palettecopy
export(first.depends)
export(palettecopy.commands)
QMAKE_EXTRA_TARGETS += first palettecopy
