PLUGIN_NAME = editlv
PLUGIN_PATH = editlv

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

DEFINES += LV_EDITLV_PLUGIN

uri = editlv

linkLocalLibrary(lvbase,      lvbase)
linkLocalLibrary(lvview,      lvview)
linkLocalLibrary(lveditor,    lveditor)
linkLocalLibrary(lvelements,  lvelements)
linkLocalLibrary(lveditqmljs, lveditqmljs)

# Source

unix:!macx{
    QMAKE_LFLAGS += \
	'-Wl,-rpath,\'\$$ORIGIN/../../link\''


    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

include($$PWD/src/editlv.pri)

OTHER_FILES += \
    $$PWD/qml/*.qml \
    $$PWD/qml/qmldir \
    $$PWD/qml/live.package.json
