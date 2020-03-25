PLUGIN_NAME = nodeeditor
PLUGIN_PATH = workspace/nodeeditor

TEMPLATE = subdirs

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

uri = workspace.nodeeditor

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes \
    samples/*.qml

DISTFILES += \
    qml/live.plugin.json \
    qml/qmldir

