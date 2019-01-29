PLUGIN_NAME = fs
PLUGIN_PATH = fs

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

DEFINES += LV_FS_PLUGIN

uri = fs

linkLocalLibrary(lvbase,   lvbase)
linkLocalLibrary(lvview,   lvview)
linkLocalLibrary(lveditor, lveditor)

# Source

include($$PWD/src/fs.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir

# Handling the palette

OTHER_FILES *= \
    palettes/*.qml

DISTFILES += \
    qml/live.package.json \
    qml/live.plugin.json
