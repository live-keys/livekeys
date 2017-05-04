
PLUGIN_NAME = lcvlive
PLUGIN_QML_DIR = $$PATH_SOURCE_PLUGINS_LIVE/qml
include($$getConfigFile(is_plugin.pri))

DEFINES += Q_LIVE_LIB

include($$PWD/src/live.pri)
include($$PWD/include/liveheaders.pri)

# Have to leave this in this notation,
# otherwise the qtcreator is getting terribly confused
OTHER_FILES += \
    $$PWD/qml/qmldir \
    $$PWD/qml/ConfigurationPanel.qml \
    $$PWD/qml/ConfigurationField.qml \
    $$PWD/qml/DropDown.qml \
    $$PWD/qml/InputBox.qml \
    $$PWD/qml/LiveCVStyle.qml \
    $$PWD/qml/LiveCVScrollStyle.qml \
    $$PWD/qml/TextButton.qml
