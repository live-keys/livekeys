PLUGIN_NAME = paper
PLUGIN_PATH = paper

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

DEFINES += LV_PAPER_LIB

uri = paper

linkLocalLibrary(lvbase, lvbase)
linkLocalLibrary(lvview, lvview)

# Source

unix:!macx{
    QMAKE_LFLAGS += \
        '-Wl,-rpath,\'\$$ORIGIN/../../link\''

    createlinkdir.commands += $${QMAKE_MKDIR_CMD} $$shell_path($${DEPLOY_PATH}/link)
    QMAKE_EXTRA_TARGETS    += createlinkdir
    POST_TARGETDEPS        += createlinkdir
}

include($$PWD/3rdparty/paperqml/src/paper.pri)
include($$PWD/src/papersrc.pri)


# Copy all js and qml files

paperjscopy.commands = $$deployDirCommand($$PWD/3rdparty/paperqml/js, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_NAME/paperjs)
paperqmlcopy.commands = $$deployDirCommand($$PWD/3rdparty/paperqml/qml, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH/paperqml)
papergrapherjscopy.commands = $$deployDirCommand($$PWD/3rdparty/js, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_NAME/papergrapherjs)
papergrapherqmlcopy.commands = $$deployDirCommand($$PWD/3rdparty/qml, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH/papergrapherqml)
fontcopy.commands = $$deployDirCommand($$PWD/3rdparty/samples/fonts, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH/fonts)
first.depends = $(first) paperjscopy paperqmlcopy papergrapherjscopy papergrapherqmlcopy fontcopy
export(first.depends)
export(paperjscopy.commands)
export(paperqmlcopy.commands)
export(papergrapherjscopy.commands)
export(papergrapherqmlcopy.commands)
export(fontcopy.commands)
QMAKE_EXTRA_TARGETS += first paperjscopy paperqmlcopy papergrapherjscopy papergrapherqmlcopy fontcopy


OTHER_FILES += \
    $$PWD/qml/*.qml \
    $$PWD/qml/tools/*.qml \
    $$PWD/3rdparty/qml/*.qml \
    $$PWD/3rdparty/qml/tools/*.js \
    $$PWD/3rdparty/qml/tools/*.qml \
    $$PWD/3rdparty/paperqml/js/*.js \
    $$PWD/3rdparty/paperqml/qml/*.qml \
    $$PWD/qml/live.package.json \
    $$PWD/qml/live.plugin.json \
    $$PWD/qml/qmldir
