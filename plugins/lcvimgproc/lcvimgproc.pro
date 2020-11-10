PLUGIN_NAME = lcvimgproc
PLUGIN_PATH = lcvimgproc

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = lcvimgproc

DEFINES += Q_LCV

# Dependencies

linkLocalLibrary(lvbase, lvbase)
linkLocalLibrary(lvview, lvview)

linkLocalPlugin(base,    base, base/baseqml)
linkLocalPlugin(live,    live)
linkLocalPlugin(lcvcore, lcvcore)

# Deploying qml is handled by the plugin.pri configuration

# Source

unix:!macx{
    QMAKE_LFLAGS += \
	'-Wl,-rpath,\'\$$ORIGIN/../../link\''


    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

include($$PWD/src/lcvimgproc.pri)
include($$PROJECT_ROOT/project/3rdparty/opencv.pri)

# Deploy samples

samplescopy.commands = $$deployDirCommand($$PWD/samples, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH/samples)
first.depends = $(first) samplescopy
export(first.depends)
export(samplescopy.commands)
QMAKE_EXTRA_TARGETS += first samplescopy

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes \
    doc/*.md \
    samples/*.qml \
    qml/palettes/*.qml

DISTFILES += \
    qml/Resize.qml \
    qml/ResizeTool.qml \
    qml/ResizeWithAspect.qml \
    qml/Rotate.qml \
    qml/RotateTool.qml \
    qml/Scale.qml \
    qml/live.package.json \
    qml/live.plugin.json
