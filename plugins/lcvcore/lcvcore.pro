PLUGIN_NAME = lcvcore
PLUGIN_PATH = lcvcore

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = lcvcore

DEFINES += Q_LCV
DEFINES += Q_LCVCORE_LIB

# Dependencies

linkLocalLibrary(lvbase,      lvbase)
linkLocalLibrary(lvview,      lvview)
linkLocalLibrary(lveditor,    lveditor)
linkLocalLibrary(lveditqmljs, lveditqmljs)

linkLocalPlugin(live, live)

# Source

unix:!macx{
    QMAKE_LFLAGS += \
	'-Wl,-rpath,\'\$$ORIGIN/../../link\''


    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

include($$PWD/src/lcvcore.pri)
include($$PWD/include/lcvcoreheaders.pri)
include($$PROJECT_ROOT/project/3rdparty/opencv.pri)
deployOpenCV()

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes


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

DISTFILES += \
    palettes/ImageFilePalette.qml \
    qml/EditCvExtension.qml \
    qml/MatViewPane.qml \
    qml/live.package.json \
    qml/live.plugin.json

