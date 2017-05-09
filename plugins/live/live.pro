TEMPLATE = lib
TARGET   = live
QT      += qml quick
CONFIG  += qt plugin

OTHER_FILES = qmldir

DEFINES += Q_LIVE_LIB

# Deploy qml

PLUGIN_DEPLOY_FROM = $$PWD/qml
win32:CONFIG(debug, debug|release): PLUGIN_DEPLOY_TO = $$OUT_PWD/../../application/debug/plugins/live
else:win32:CONFIG(release, debug|release): PLUGIN_DEPLOY_TO = $$OUT_PWD/../../application/release/plugins/live
else:unix: PLUGIN_DEPLOY_TO = $$OUT_PWD/../../application/plugins/live

PALETTE_DEPLOY_FROM = $$PWD/palettes
PALETTE_DEPLOY_TO = $$PLUGIN_DEPLOY_TO/palettes

win32:PLUGIN_DEPLOY_TO ~= s,/,\\,g
win32:PLUGIN_DEPLOY_FROM ~= s,/,\\,g

win32:PALETTE_DEPLOY_FROM ~= s,/,\\,g
win32:PALETTE_DEPLOY_TO ~= s,/,\\,g

plugincopy.commands = $(COPY_DIR) \"$$PLUGIN_DEPLOY_FROM\" \"$$PLUGIN_DEPLOY_TO\"
palettecopy.commands = $(COPY_DIR) \"$$PALETTE_DEPLOY_FROM\" \"$$PALETTE_DEPLOY_TO\"

first.depends = $(first) plugincopy palettecopy
export(first.depends)
export(plugincopy.commands)
export(palettecopy.commands)

QMAKE_EXTRA_TARGETS += first plugincopy palettecopy

# Destination

win32:CONFIG(debug, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../../application/debug/plugins/live)
else:win32:CONFIG(release, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../../application/release/plugins/live)
else:unix: TARGET = $$quote($$OUT_PWD/../../application/plugins/live)

#PLUGIN_NAME = live
#PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_LIVE/qml
#include($$getConfigFile(is_plugin.pri))

uri = plugins.live

DEFINES += Q_LIVE_LIB

include($$PWD/src/live.pri)
include($$PWD/include/liveheaders.pri)

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir
#    qml/projects.qmltypes

# Handling the palette

OTHER_FILES *= \
    palettes/palettedir \
    palettes/*.qml

#PALETTES_DEPLOY_FROM         = $$shell_path($$PATH_SOURCE_PLUGINS_LIVE/palettes)
#PLUGIN_DEPLOY_TO        = $$shell_path($$PATH_DEPLOY_PLUGINS/$$PLUGIN_NAME)

#!exists($$PALETTES_DEPLOY_FROM){
#    warning(Expected folder $$PALETTES_DEPLOY_FROM)
#    copy.commands =
#} else {
#    copy.commands = $$QMAKE_COPY_DIR $$PALETTES_DEPLOY_FROM $$PLUGIN_DEPLOY_TO
#}

#palette_first.depends = $(palette_first) copy
#export(palette_first.depends)
#export(copy.commands)

#QMAKE_EXTRA_TARGETS += palette_first copy
