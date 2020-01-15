PLUGIN_NAME = lcvphoto
PLUGIN_PATH = lcvphoto

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = lcvphoto

DEFINES += Q_LCV

# Dependencies

linkLocalLibrary(lvbase,      lvbase)
linkLocalLibrary(lvview,      lvview)
linkLocalLibrary(lveditor,    lveditor)
linkLocalLibrary(lveditqmljs, lveditqmljs)

linkLocalPlugin(live,    live)
linkLocalPlugin(lcvcore, lcvcore)

# Qml Files
# Deploying qml is handled by the plugin.pri configuration

OTHER_FILES *= \
    qml/qmldir \
    qml/plugins.qmltypes

# Source

unix:!macx{
    QMAKE_LFLAGS += \
	'-Wl,-rpath,\'\$$ORIGIN/../../link\''


    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

include($$PWD/src/lcvphoto.pri)
include($$PROJECT_ROOT/project/3rdparty/opencv.pri)

# Handling the palettes

OTHER_FILES *= \
    palettes/*.qml \
    doc/*.md

# Deploy the palettes

palettecopy.commands = $$deployDirCommand($$PWD/palettes, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_NAME/palettes)
first.depends = $(first) palettecopy
export(first.depends)
export(palettecopy.commands)
QMAKE_EXTRA_TARGETS += first palettecopy

# Distfiles

DISTFILES += \
    qml/FastNlMeansDenoising_.qml \
    qml/LevelsSliders.qml \
    qml/HueSaturationLightnessSliders.qml \
    qml/live.package.json \
    qml/live.plugin.json \
    palettes/LevelsPalette.qml
