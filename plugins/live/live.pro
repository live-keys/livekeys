#PLUGIN_NAME = live
#include($$getConfigFile(is_plugin.pri))

TEMPLATE = lib
TARGET   = live
QT      += qml quick
CONFIG  += qt plugin

DEFINES += Q_LIVE_LIB

# Deploy qml

qmlcopy.commands     = $$deployLocalDirCommand($$PWD/qml, plugins/live)
palettecopy.commands = $$deployLocalDirCommand($$PWD/palettes, plugins/live/palettes)
first.depends = $(first) qmlcopy palettecopy
export(first.depends)
export(qmlcopy.commands)
export(palettecopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy palettecopy

# Destination

win32:DLLDESTDIR = $$buildModePath($$DEPLOY_PWD)/plugins/live
else:DESTDIR = $$buildModePath($$DEPLOY_PWD)/plugins/live

uri = plugins.live


include($$PWD/src/live.pri)
include($$PWD/include/liveheaders.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir
#    qml/plugins.qmltypes

# Handling the palette

OTHER_FILES *= \
    palettes/palettedir \
    palettes/*.qml

