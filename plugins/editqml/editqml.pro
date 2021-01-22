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

unix:!macx{
    QMAKE_LFLAGS += \
	'-Wl,-rpath,\'\$$ORIGIN/../../link\''


    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

include($$PWD/src/editqml.pri)

OTHER_FILES += \
    $$PWD/qml/*.qml \
    $$PWD/qml/qmldir \
    $$PWD/qml/live.package.json

DISTFILES += \
    qml/AddQmlBox.qml \
    qml/ObjectContainer.qml \
    qml/ObjectContainerTop.qml \
    qml/PaletteControls.qml \
    qml/PaletteGroup.qml \
    qml/PaletteStyle.qml \
    qml/PropertyContainer.qml \
    qml/PaletteContainer.qml \
    qml/ShapingOptions.qml \
    qml/live.plugin.json \
    qml/plugininfo.qml \
    qml/plugins.qmltypes
