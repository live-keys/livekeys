PLUGIN_NAME = editqml
PLUGIN_PATH = editqml

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

DEFINES += LV_EDITQML_PLUGIN

uri = editor

linkLocalLibrary(lvbase,      lvbase)
linkLocalLibrary(lvview,      lvview)
linkLocalLibrary(lveditor,    lveditor)
linkLocalLibrary(lveditqmljs, lveditqmljs)

# Source

include($$PWD/src/editqml.pri)

OTHER_FILES += \
    $$PWD/qml/*.qml \
    $$PWD/qml/qmldir \
    $$PWD/qml/live.package.json

#DISTFILES += \
#    qml/DocumentFragment.qml \
#    qml/PaletteList.qml \
#    qml/RefactorList.qml \
#    qml/AddBox.qml

DISTFILES += \
    qml/AddQmlBox.qml
