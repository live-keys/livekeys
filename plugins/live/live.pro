PLUGIN_NAME = live
PLUGIN_PATH = live

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

DEFINES += Q_LIVE_LIB

uri = live

linkLocalLibrary(lvbase,   lvbase)
linkLocalLibrary(lveditor, lveditor)

# Source

include($$PWD/src/live.pri)
include($$PWD/include/liveheaders.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes

DISTFILES += \
    qml/VisualLogView.qml \
    qml/SliderLabel.qml

