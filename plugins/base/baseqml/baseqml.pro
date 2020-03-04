PLUGIN_NAME = base
PLUGIN_PATH = base

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

DEFINES += LV_BASE_PLUGIN

uri = base

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

include($$PWD/src/baseqml.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir

# Handling the palette

OTHER_FILES *= \
    palettes/*.qml

DISTFILES += \
    qml/live.package.json \
    qml/live.plugin.json \
    qml/plugins.qmltypes
