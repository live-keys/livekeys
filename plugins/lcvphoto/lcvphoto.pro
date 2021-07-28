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
    doc/*.md \
    samples/*.qml

# Deploy the palettes and samples

palettecopy.commands = $$deployDirCommand($$PWD/palettes, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_NAME/palettes)
samplescopy.commands = $$deployDirCommand($$PWD/samples, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH/samples)
first.depends = $(first) palettecopy samplescopy
export(first.depends)
export(samplescopy.commands)
export(palettecopy.commands)
QMAKE_EXTRA_TARGETS += first palettecopy samplescopy

# Distfiles

DISTFILES += \
    palettes/HueSaturationLightnessPalette.qml \
    qml/AlignMTB.qml \
    qml/BrightnessAndContrast.qml \
    qml/CalibrateDebevec.qml \
    qml/CalibrateRobertson.qml \
    qml/DenoiseTVL1.qml \
    qml/FastNlMeansDenoising_.qml \
    qml/HueSaturationLightness.qml \
    qml/Levels.qml \
    qml/LevelsSliders.qml \
    qml/HueSaturationLightnessSliders.qml \
    qml/MergeDebevec.qml \
    qml/MergeRobertson.qml \
    qml/Sepia.qml \
    qml/Stitcher.qml \
    qml/Temperature.qml \
    qml/TonemapDrago.qml \
    qml/TonemapMantiuk.qml \
    qml/TonemapReinhard.qml \
    qml/live.package.json \
    qml/live.plugin.json \
    palettes/LevelsPalette.qml \
    palettes/LevelsDefaultPalette.json
