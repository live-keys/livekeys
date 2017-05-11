#PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_FEATURES2D/qml
#include($$getConfigFile(is_plugin.pri))

PLUGIN_NAME = lcvfeatures2d
PLUGIN_PATH = lcvfeatures2d

TEMPLATE = lib
TARGET   = $$PLUGIN_NAME
QT      += qml quick
CONFIG  += qt plugin

uri = plugins.lcvfeatures2d

DEFINES += Q_LCV
DEFINES += Q_LCVFEATURES2D_LIB

# Destination

win32:DLLDESTDIR = $$buildModePath($$DEPLOY_PWD)/plugins/$$PLUGIN_PATH
else:DESTDIR = $$buildModePath($$DEPLOY_PWD)/plugins/$$PLUGIN_PATH

# Dependencies

linkLocalPlugin(live,    live)
linkLocalPlugin(lcvcore, lcvcore)

# Deploy qml

qmlcopy.commands = $$deployLocalDirCommand($$PWD/qml, plugins/$$PLUGIN_PATH)
first.depends = $(first) qmlcopy
export(first.depends)
export(qmlcopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy

# Source

include($$PWD/src/lcvfeatures2d.pri)
include($$PWD/include/lcvfeatures2dheaders.pri)
include($$PROJECT_ROOT/3rdparty/opencvconfig.pro)

deployOpenCV()

OTHER_FILES *= \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes
