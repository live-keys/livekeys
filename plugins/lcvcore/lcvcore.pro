PLUGIN_NAME = lcvcore
PLUGIN_PATH = $$PWD

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = plugins.lcvcore

DEFINES += Q_LCV
DEFINES += Q_LCVCORE_LIB

# Dependencies

linkLocalPlugin(live, live)

# Source

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
    palettes/palettedir \
    palettes/*.qml

# Deploy the palettes

palettecopy.commands = $$deployDirCommand($$PWD/palettes, $$PATH_DEPLOY_PLUGINS/$$PLUGIN_NAME/palettes)
first.depends = $(first) palettecopy
export(first.depends)
export(palettecopy.commands)
QMAKE_EXTRA_TARGETS += first palettecopy
