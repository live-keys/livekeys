PLUGIN_NAME = icons
PLUGIN_PATH = workspace/icons
uri = workspace.icons

QT += svg

# PLUGIN_NAME and PLUGIN_PATH must be set up prior to including this config file
include($$getGlobalFile(plugin.pri))

# Deploy the samples

qmlcopy.commands = $$deployDirCommand($$PLUGIN_QML_DIR, $$PLUGIN_DEPLOY_PATH/$$PLUGIN_PATH)
first.depends = $(first) qmlcopy
export(first.depends)
export(qmlcopy.commands)
QMAKE_EXTRA_TARGETS += first qmlcopy

include($$PWD/src/icons.pri)

OTHER_FILES += \
    qml/*.qml \
    qml/qmldir \
    qml/plugins.qmltypes

DISTFILES += \
    qml/PlusIcon.qml \
    qml/RecordIcon.qml \
    qml/StopIcon.qml \
    qml/live.plugin.json \
    qml/qmldir

