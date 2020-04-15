PLUGIN_NAME = quickqanava
PLUGIN_PATH = workspace/quickqanava

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = workspace.quickqanava

linkLocalLibrary(lvbase, lvbase)
linkLocalLibrary(lveditor, lveditor)
linkLocalLibrary(lveditqmljs, lveditqmljs)

include($$PWD/src/quickqanava.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes

DISTFILES += \
    qml/live.plugin.json

