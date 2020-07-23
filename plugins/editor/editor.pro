PLUGIN_NAME = editor
PLUGIN_PATH = editor

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

DEFINES += LV_EDITOR_PLUGIN

uri = editor

linkLocalLibrary(lvbase,   lvbase)
linkLocalLibrary(lvview,   lvview)
linkLocalLibrary(lveditor, lveditor)

# Source

unix:!macx{
    QMAKE_LFLAGS += \
	'-Wl,-rpath,\'\$$ORIGIN/../../link\''


    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

include($$PWD/src/editor.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes

OTHER_FILES += \
    qml/style/*.qml \
    qml/style/qmldir \
    qml/style/plugins.qmltypes \
    qml/style/live.plugin.json

# Handling the palette

OTHER_FILES *= \
    palettes/*.qml

# Deploy The palette

palettecopy.commands = $$deployDirCommand($$PWD/palettes, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_NAME/palettes)
samplescopy.commands = $$deployDirCommand($$PWD/samples, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH/samples)
first.depends = $(first) palettecopy samplescopy
export(first.depends)
export(samplescopy.commands)
export(palettecopy.commands)
QMAKE_EXTRA_TARGETS += first palettecopy samplescopy

DISTFILES += \
    palettes/BooleanPalette.qml \
    palettes/ConnectionPalette.qml \
    palettes/ImportsPalette.qml \
    palettes/IntHistoryPlotPalette.qml \
    palettes/DoubleHistoryPlotPalette.qml \
    palettes/NodePalette.qml \
    palettes/PathPalette.qml \
    palettes/RectangleSizePalette.json \
    palettes/SizePalette.qml \
    palettes/TextPalette.qml \
    palettes/TriggerPalette.qml \
    qml/DocumentationView.qml \
    qml/EditorPane.qml \
    qml/ObjectPalettePane.qml \
    qml/PaletteConnection.qml \
    qml/WorkspaceControl.qml \
    qml/live.package.json \
    qml/live.plugin.json \
    qml/PaletteListView.qml \
    palettes/EditPalette.qml \
    qml/Pane.qml \
    qml/PaneWindow.qml \
    qml/WorkspaceTheme.qml \
    qml/PaneMenuButton.qml \
    qml/LoadingAnimation.qml \
    qml/loaddocs.qml \
    qml/loadqtdocs.qml
