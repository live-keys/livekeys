TEMPLATE = lib
TARGET   = live
QT      += qml quick
CONFIG  += qt plugin

OTHER_FILES = qmldir

DEFINES += Q_LIVE_LIB

# Deploy qml

PLUGIN_DEPLOY_FROM = $$PWD/qml
win32:CONFIG(debug, debug|release): PLUGIN_DEPLOY_TO = $$OUT_PWD/../application/debug/live
else:win32:CONFIG(release, debug|release): PLUGIN_DEPLOY_TO = $$OUT_PWD/../application/release/live
else:unix: PLUGIN_DEPLOY_TO = $$OUT_PWD/../../application/live

win32:PLUGIN_DEPLOY_TO ~= s,/,\\,g
win32:PLUGIN_DEPLOY_FROM ~= s,/,\\,g

plugincopy.commands = $(COPY_DIR) \"$$PLUGIN_DEPLOY_FROM\" \"$$PLUGIN_DEPLOY_TO\"

first.depends = $(first) plugincopy
export(first.depends)
export(plugincopy.commands)

QMAKE_EXTRA_TARGETS += first plugincopy

# Destination

win32:CONFIG(debug, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../application/debug/live)
else:win32:CONFIG(release, debug|release): DLLDESTDIR = $$quote($$OUT_PWD/../application/release/live)
else:unix: TARGET = $$quote($$OUT_PWD/../../application/live)

include($$PWD/src/live.pri)
include($$PWD/include/liveheaders.pri)

OTHER_FILES += \
    $$PWD/qml/qmldir \
    $$PWD/qml/ConfigurationPanel.qml \
    $$PWD/qml/ConfigurationField.qml \
    $$PWD/qml/DropDown.qml \
    $$PWD/qml/InputBox.qml \
    $$PWD/qml/LiveCVStyle.qml \
    $$PWD/qml/LiveCVScrollStyle.qml \
    $$PWD/qml/TextButton.qml
