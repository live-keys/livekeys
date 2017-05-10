#PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_CORE/qml
#include($$getConfigFile(is_plugin.pri))

PLUGIN_NAME = lcvcore
PLUGIN_PATH = lcvcore

TEMPLATE = lib
TARGET   = $$PLUGIN_NAME
QT      += qml quick
CONFIG  += qt plugin

uri = plugins.lcvcore

DEFINES += Q_LCV
DEFINES += Q_LCVCORE_LIB

# Destination

win32:DLLDESTDIR = $$buildModePath($$DEPLOY_PWD)/plugins/$$PLUGIN_PATH
else:DESTDIR = $$buildModePath($$DEPLOY_PWD)/plugins/$$PLUGIN_PATH

# Dependencies

linkLocalPlugin(live, live)

# Deploy qml

qmlcopy.commands = $$deployLocalDirCommand($$PWD/qml, plugins/$$PLUGIN_PATH)
first.depends = $(first) qmlcopy
export(first.depends)
export(qmlcopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy

# Source

include($$PWD/src/lcvcore.pri)
include($$PWD/include/lcvcoreheaders.pri)
include($$PROJECT_ROOT/3rdparty/opencvconfig.pro)
deployOpenCV()

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes


