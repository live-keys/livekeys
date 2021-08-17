PLUGIN_NAME = table
PLUGIN_PATH = table

CONFIG += c++14

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = table

linkLocalLibrary(lvbase,    lvbase)
linkLocalLibrary(lvview,    lvview)
linkLocalLibrary(lveditor,  lveditor)

include($$PWD/3rdparty/csvparser.pri)

# Source

unix:!macx{
    QMAKE_LFLAGS += \
        '-Wl,-rpath,\'\$$ORIGIN/../../link\''

    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

include($$PWD/src/table.pri)

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes

DISTFILES += \
    qml/TableEditor.qml \
    qml/TableEditorStyle.qml
