TEMPLATE = lib
TARGET   = lcvcore
QT      += qml quick
CONFIG  += qt plugin

TARGET = $$qtLibraryTarget($$TARGET)
uri = plugins.lcvcore

DEFINES += Q_LCV
DEFINES += Q_LCVCORE_LIB

include($$PWD/../use_liblive.pri)
include($$PWD/src/lcvcore.pri)
include($$PWD/include/lcvcoreheaders.pri)
include($$PWD/../../3rdparty/opencvconfig.pro)
deployOpenCV()

# Destination

win32:CONFIG(debug, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../../application/debug/plugins/lcvcore)
else:win32:CONFIG(release, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../../application/release/plugins/lcvcore)
else:unix: TARGET = $$quote($$OUT_PWD/../../application/plugins/lcvcore)

# Qml

OTHER_FILES = \
    $$PWD/qml/qmldir \
    $$PWD/qml/plugins.qmltypes \
    $$PWD/qml/RegionSelection.qml \
    $$PWD/qml/VideoControls.qml

# Deploy qml

PLUGIN_DEPLOY_FROM = $$PWD/qml
win32:CONFIG(debug, debug|release): PLUGIN_DEPLOY_TO = $$OUT_PWD/../../application/debug/plugins/lcvcore
else:win32:CONFIG(release, debug|release): PLUGIN_DEPLOY_TO = $$OUT_PWD/../../application/release/plugins/lcvcore
else:unix: PLUGIN_DEPLOY_TO = $$OUT_PWD/../../application/plugins/lcvcore

win32:PLUGIN_DEPLOY_TO ~= s,/,\\,g
win32:PLUGIN_DEPLOY_FROM ~= s,/,\\,g

plugincopy.commands = $(COPY_DIR) \"$$PLUGIN_DEPLOY_FROM\" \"$$PLUGIN_DEPLOY_TO\"

first.depends = $(first) plugincopy
export(first.depends)
export(plugincopy.commands)

QMAKE_EXTRA_TARGETS += first plugincopy
