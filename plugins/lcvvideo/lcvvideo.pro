#PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_VIDEO/qml
#include($$getConfigFile(is_plugin.pri))

PLUGIN_NAME = lcvvideo
PLUGIN_PATH = lcvvideo

TEMPLATE = lib
TARGET   = $$PLUGIN_NAME
QT      += qml quick
CONFIG  += qt plugin

uri = plugins.lcvvideo

DEFINES += Q_LCV
DEFINES += Q_LCVVIDEO_LIB

# Destination

win32:DLLDESTDIR = $$buildModePath($$DEPLOY_PWD)/plugins/$$PLUGIN_PATH
else:DESTDIR = $$buildModePath($$DEPLOY_PWD)/plugins/$$PLUGIN_PATH

# Dependencies

linkLocalPlugin(live, live)
linkLocalPlugin(lcvcore, lcvcore)

# Deploy qml

qmlcopy.commands = $$deployLocalDirCommand($$PWD/qml, plugins/$$PLUGIN_PATH)
first.depends = $(first) qmlcopy
export(first.depends)
export(qmlcopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy

# Source

include($$PWD/src/lcvvideo.pri)
include($$PWD/include/lcvvideoheaders.pri)
include($$PROJECT_ROOT/3rdparty/opencvconfig.pro)
deployOpenCV()

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes
