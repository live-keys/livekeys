PLUGIN_NAME = editor
PLUGIN_PATH = editor

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

DEFINES += LV_EDITOR_PLUGIN

uri = editor

linkLocalLibrary(lvbase,   lvbase)
linkLocalLibrary(lveditor, lveditor)

# Source

include($$PWD/src/editor.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes

# Handling the palette

OTHER_FILES *= \
    palettes/palettedir \
    palettes/*.qml

# Deploy The palette

palettecopy.commands = $$deployDirCommand($$PWD/palettes, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_NAME/palettes)
first.depends = $(first) palettecopy
export(first.depends)
export(palettecopy.commands)
QMAKE_EXTRA_TARGETS += first palettecopy

DISTFILES += \
    qml/DocumentFragment.qml \
    palettes/IntHistoryPlotPalette.qml \
    palettes/DoubleHistoryPlotPalette.qml \
    qml/PaletteList.qml \
    qml/RefactorList.qml
