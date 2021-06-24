PLUGIN_NAME = base
PLUGIN_PATH = base

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

DEFINES += LV_BASEQML_LIB

uri = base

linkLocalLibrary(lvbase,      lvbase)
linkLocalLibrary(lvview,      lvview)
linkLocalLibrary(lveditor,    lveditor)
linkLocalLibrary(lveditqmljs, lveditqmljs)

# Source

unix:!macx{
    QMAKE_LFLAGS += \
        '-Wl,-rpath,\'\$$ORIGIN/../../link\''

    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

include($$PWD/src/baseqml.pri)
include($$PWD/include/baseqmlheaders.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir

# Handling the palette

OTHER_FILES *= \
    palettes/*.qml \
    samples/*.qml

# Deploy

palettecopy.commands = $$deployDirCommand($$PWD/palettes, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_NAME/palettes)
samplescopy.commands = $$deployDirCommand($$PWD/samples, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH/samples)
first.depends = $(first) palettecopy samplescopy
export(first.depends)
export(samplescopy.commands)
export(palettecopy.commands)
QMAKE_EXTRA_TARGETS += first palettecopy samplescopy


DISTFILES += \
    palettes/ActTriggerTypePalette.qml \
    palettes/StreamValuePalette.json \
    qml/ConvertToInt.qml \
    qml/JsonDecoder.qml \
    qml/JsonEncoder.qml \
    qml/TextClip.qml \
    qml/TextLineAtPosition.qml \
    qml/live.package.json \
    qml/live.plugin.json \
    qml/plugins.qmltypes \
    samples/readjsonfile.qml
