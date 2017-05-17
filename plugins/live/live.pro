#include($$getConfigFile(is_plugin.pri))

PLUGIN_NAME = live
PLUGIN_PATH = live

TEMPLATE = lib
TARGET   = $$PLUGIN_NAME
QT      += qml quick
CONFIG  += qt plugin

DEFINES += Q_LIVE_LIB

# Destination

win32:DLLDESTDIR = $$buildModePath($$DEPLOY_PWD)
else:DESTDIR = $$buildModePath($$DEPLOY_PWD)

uri = plugins.live

linkLocalLibrary(editor/lcveditor, lcveditor)

# Deploy qml

qmlcopy.commands     = $$deployLocalDirCommand($$PWD/qml, plugins/$$PLUGIN_PATH)
palettecopy.commands = $$deployLocalDirCommand($$PWD/palettes, plugins/$$PLUGIN_PATH/palettes)
first.depends = $(first) qmlcopy palettecopy
export(first.depends)
export(qmlcopy.commands)
export(palettecopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy palettecopy

# Source

include($$PWD/src/live.pri)
include($$PWD/include/liveheaders.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes

# Handling the palette

OTHER_FILES *= \
    palettes/palettedir \
    palettes/*.qml

