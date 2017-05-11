#PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_IMGPROC/qml
#include($$getConfigFile(is_plugin.pri))

PLUGIN_NAME = lcvimgproc
PLUGIN_PATH = lcvimgproc

TEMPLATE = lib
TARGET   = $$PLUGIN_NAME
QT      += qml quick
CONFIG  += qt plugin

uri = plugins.lcvimgproc

DEFINES += Q_LCV

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

include($$PWD/src/lcvimgproc.pri)
include($$PROJECT_ROOT/3rdparty/opencvconfig.pro)
deployOpenCV()

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes
